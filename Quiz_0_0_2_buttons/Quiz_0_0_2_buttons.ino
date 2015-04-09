#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Hardware configuration
int thisbutton = 1;

const int RedLed = 8;
const int GreenLed = 7;
const int BlueLed = 6;
const int BigButton = 4;
const int ButtonA = A0;
const int ButtonB = A1;
const int ButtonC = A2;
const int ButtonD = A3;

unsigned long answer = 0;

int buttonStateA = 0; 
int buttonStateB = 0; 
int buttonStateC = 0; 
int buttonStateD = 0; 
int BigButtonState = 0;
int BigButtonReading;
int LastBigButtonState = LOW;
long LastDebounceTimeBig = 0;
long DebounceDelay = 40;
boolean Answered = false;
boolean Reset = false;
int others[2];
boolean other_answers = false;
int Master_Reset;
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
//define pin modi
  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(BlueLed, OUTPUT);
  pinMode(BigButton, INPUT);
  pinMode(ButtonA, INPUT);
  pinMode(ButtonB, INPUT);
  pinMode(ButtonC, INPUT);
  pinMode(ButtonD, INPUT);
  
  Serial.begin(57600);
  printf_begin();
  printf("ROLE: %s\n\r","Buttons");
 
  radio.begin(); // Setup and configure rf radio
  radio.setRetries(30,15); // optionally, increase the delay between retries & # of retries
  radio.setPayloadSize(8); // optionally, reduce the payload size.  seems to improve reliability

  // Open pipes to other nodes for communication
  // This simple sketch opens two pipes for these two nodes to communicate back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  
  radio.startListening();   // Start listening
  radio.printDetails(); // Dump the configuration of the rf unit for debugging
  
}

void loop(void)
//games will begin in modus 1, you can choosse an other modus after everyone has pushed his button
{
  Push();
  if (Answered == true)
   {
     radio.startListening(); // luister eerst gedurende 200ms als niemand anders aan het antwoorden is
     unsigned long started_listening = millis();
     while (millis()-started_listening < 200)
     {
       while (!other_answers)
       {
         other_answers = radio.read(others, sizeof(others));
        }
     }
     if (other_answers == true)//als er andere antwoorden zijn
     {
     while (!Reset)
       {
       digitalWrite(RedLed, HIGH);//laat bezet led branden en wacht op reset
       Master_Reset = radio.read(others, sizeof(others));
       if(Master_Reset = 1111)
         {
         Reset = true;
         }
       }
     }
     else // stuur antwoord maar 
     {
       radio.stopListening(); // First, stop listening so we can talk. 
       unsigned long time = millis(); // Take the time, and send it.  This will block until complete
       printf("Now sending %lu...",answer);
       radio.write( &answer , sizeof(answer) );
       radio.startListening(); // Now, continue listening
       unsigned long started_waiting_at = millis(); // Wait here until we get a response, or timeout (250ms)
       bool timeout = false;
       while ( ! radio.available() && ! timeout )
       if (millis() - started_waiting_at > 100 )
       timeout = true;
        // Describe the results
      if ( timeout )
      {
        printf("Failed, response timed out.\n\r");
      }
      else
      {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,started_waiting_at-millis()); // Spew it
    }
     }
   } 
}


void Push()//debounce if the big button is used
{
  BigButtonReading = digitalRead(BigButton);
  if(BigButtonReading != LastBigButtonState) 
  {
    LastDebounceTimeBig = millis();
  }
  if(millis() - LastDebounceTimeBig > DebounceDelay) 
  {
    if(BigButtonReading != BigButtonState)
    {
      BigButtonState = BigButtonReading;
      if (BigButtonState == HIGH)
        {  
        //test with led
        answer = 00 & thisbutton & 11 & 1;
        Answered = true;
        }
        else
        Answered = false;
    }
  }
LastBigButtonState = BigButtonReading;
}


void Options()
{
   buttonStateA = digitalRead(ButtonA);
  buttonStateB = digitalRead(ButtonB);
  buttonStateC = digitalRead(ButtonC);
  buttonStateD = digitalRead(ButtonD);
  if (buttonStateA == HIGH ||  buttonStateC == HIGH || buttonStateD == HIGH ) 
  {
    digitalWrite(RedLed, HIGH);
    answer = 1;
  }
  else if (buttonStateB == HIGH) 
  {
    digitalWrite(BlueLed, HIGH);
    answer = 2;
  }
  else
  {
    digitalWrite(RedLed, LOW);
    digitalWrite(BlueLed, LOW);
  } 
}
