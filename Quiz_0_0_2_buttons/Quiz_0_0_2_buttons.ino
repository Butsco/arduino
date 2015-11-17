#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Hardware configuration
int thisbutton = 1;

const int RGB[3] = {6,8,7};

const long DEBOUNCE = 50;
int READING = LOW;

const int BUTTON[4] = {4, A3, A2, A1};
int state[4] = {LOW, LOW, LOW, LOW};

int led_state[4] = {LOW, LOW, LOW, LOW};

int last[4] = {LOW, LOW, LOW, LOW};

long last_received[4] = {0, 0, 0, 0};

long debounce[4] = {0, 0, 0, 0};


char MODE = 1; 
char SERVER = 0;
char CLIENT_1 = 1;
char CLIENT_2 = 2;
char CLIENT_3 = 3;
char CLIENT_4 = 4;

// messages
char SERVER_RESET = 0;
char SERVER_WINNER_1 = 1;
char SERVER_WINNER_2 = 2;
char SERVER_WINNER_3 = 3;
char SERVER_WINNER_4 = 4;

char CLIENT_1_PUSH = 10;
char CLIENT_2_PUSH = 20;
char CLIENT_3_PUSH = 30;
char CLIENT_4_PUSH = 40;

char SERVER_WINNER[4] = {1,2,3,4};

// 0 = server
// 1-4 client

// messages 0-9 are from server
// messages 10 < 20 are from client 1
// ..

char WINNER = 0;


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
  Serial.println("Hoppa");

  

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
  
}

void loop(void)
{
  
  checkButtons();
  
  long now = millis();
  
  while( radio.available() ) {
      Serial.println("READ");
    
      char x;
      radio.read( &x, sizeof(char) );
      
      Serial.println(x, DEC);
  
      if(MODE == SERVER) {
        if(WINNER == SERVER && x > SERVER) {
          
          if(x == CLIENT_1_PUSH) {
            WINNER = CLIENT_1;
            sendStuff(SERVER_WINNER_1);
          } else if(x == CLIENT_2_PUSH) {
            WINNER = CLIENT_2;
            sendStuff(SERVER_WINNER_2);
          } else if(x == CLIENT_3_PUSH) {
            WINNER = CLIENT_3;
            sendStuff(SERVER_WINNER_3);
          } else if(x == CLIENT_4_PUSH) {
            WINNER = CLIENT_4;
            sendStuff(SERVER_WINNER_4);
          }
          
          last_received[WINNER] = millis() + 99999999999;
        } 
      } else {
        
        WINNER = 0;
        if(x == SERVER_WINNER_1) {
            WINNER = CLIENT_1;
        } else if(x == SERVER_WINNER_2) {
            WINNER = CLIENT_2;
         } else if(x == SERVER_WINNER_3) {
            WINNER = CLIENT_3;
         } else if(x == SERVER_WINNER_4) {
            WINNER = CLIENT_4;
         } 
         
         if(WINNER > SERVER) {
           
           if(WINNER == MODE) {
             last_received[0] = millis() + 5000;
           }
           
           last_received[WINNER] = millis() + 3000;
         }
      
      }
      
      //last_received[x] = millis() + 500;
  }
  
  for(int i=0; i<4; i++) {
    led_state[i] = 0;
    if(last_received[i] > now) {
      led_state[i] = HIGH;
    }  
    
    //led_state[i] = state[i];
  }
  
  //analogWrite(RGB[0], (state[0] | state[1])*255);
  //analogWrite(RGB[1], (state[0] | state[2])*255);
  //analogWrite(RGB[2], (state[0] | state[3])*255);
  
  analogWrite(RGB[0], (led_state[0] | led_state[1])*255);
  analogWrite(RGB[1], (led_state[0] | led_state[2])*255);
  analogWrite(RGB[2], (led_state[0] | led_state[3])*255);
  
  
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
          //sendStuff(i);
          
          if(MODE == SERVER) {
            last_received[WINNER] = SERVER;
            WINNER = SERVER;
          } else if(MODE == CLIENT_1) {
            sendStuff(CLIENT_1_PUSH);
          } else if(MODE == CLIENT_2) {
            sendStuff(CLIENT_2_PUSH);
          } else if(MODE == CLIENT_3) {
            sendStuff(CLIENT_3_PUSH);
          } else if(MODE == CLIENT_4) {
            sendStuff(CLIENT_4_PUSH);
          }
        }
      }
 
      
    }  
    
    last[i] = READING;
  }
  
 

}


void sendStuff(char x) {     
  radio.stopListening();
  radio.write( &x , sizeof(x) );
  radio.startListening();
}

