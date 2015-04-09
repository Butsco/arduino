
/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

const int RedLed = 8;
const int GreenLed = 7;
const int BlueLed = 6;
const int BigButton = 4;
const int ButtonA = A0;
const int ButtonB = A1;
const int ButtonC = A2;
const int ButtonD = A3;

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL }; // Radio pipe addresses for the 2 nodes to communicate.


void setup(void)
{
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
  printf("ROLE: %s\n\r","console");

  radio.begin();// Setup and configure rf radio
  radio.setRetries(15,15); // optionally, increase the delay between retries & # of retries
  radio.setPayloadSize(8); // optionally, reduce the payload size.  seems to improve reliability

  // Open pipes to other nodes for communication


  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening(); // Start listening
  radio.printDetails();   // Dump the configuration of the rf unit for debugging
}

void loop(void)
{
    if ( radio.available() )// if there is data ready
    {
      // Dump the payloads until we've gotten everything
      unsigned long answer;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &answer, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",answer);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
        if (answer == 1)
        {
          digitalWrite(RedLed, HIGH);
          digitalWrite(BlueLed, LOW);
        }
        else if (answer == 2)
        {
          digitalWrite(BlueLed, HIGH);
          digitalWrite(RedLed, LOW);
        }
        else
        {
          digitalWrite(RedLed, LOW);
          digitalWrite(BlueLed, LOW);
        }
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &answer, sizeof(unsigned long) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  
}

