#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Hardware configuration
int thisbutton = 1;

const int RGB[3] = {6,8,7};

const long DEBOUNCE = 50;
int READING = LOW;
const int BUTTON[4] = {4, A3, A2, A1};
int state[4] = {LOW, LOW, LOW, LOW};
int last[4] = {LOW, LOW, LOW, LOW};
long debounce[4] = {0, 0, 0, 0};

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  pinMode(RGB[0], OUTPUT);
  pinMode(RGB[1], OUTPUT);
  pinMode(RGB[2], OUTPUT);
  pinMode(BUTTON[0], INPUT);
  pinMode(BUTTON[1], INPUT);
  pinMode(BUTTON[2], INPUT);
  pinMode(BUTTON[3], INPUT);
  
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
{
  
  checkButtons();
  
  analogWrite(RGB[0], 255);
  analogWrite(RGB[1], (state[0] | state[2])*255);
  analogWrite(RGB[2], (state[0] | state[3])*255);

  return;
  
  
  
}

void checkButtons()
{
  for(int i=0; i<4; i++) {
    READING = digitalRead(BUTTON[i]);
    
    if(READING != last[i]) 
    {
        debounce[i] = millis();
        Serial.println(millis() - debounce[i]);
    }
    
    if(millis() - debounce[i] > DEBOUNCE) 
    { 
      
      if(READING != state[i]) {
        state[i] = READING;
        
        if(state[i] == HIGH) {
          sendStuff(i);
        }
      }
 
      
    }  
    
    last[i] = READING;
  }

}


void sendStuff(char x) {
/*
     radio.startListening(); // luister eerst gedurende 200ms als niemand anders aan het antwoorden is
     unsigned long started_listening = millis();
     
     while (millis()-started_listening < 200)
     {
       while (!other_answers)
       {
         other_answers = radio.read(others, sizeof(others));
        }
     }*/
     

     radio.stopListening(); // First, stop listening so we can talk. 
     unsigned long time = millis(); // Take the time, and send it.  This will block until complete
     printf("Now sending...");
 
     radio.write( &x , sizeof(x) );
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



