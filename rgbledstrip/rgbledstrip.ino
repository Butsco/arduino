#include "LPD8806.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma

int nLEDs = 2;

int dataPin  = 12;
int clockPin = 13;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

const int sensors[5] = {A0, A1, A2, A3, A4};
int values[5] = {0, 0, 0, 0, 0};

const int THRESHOLD = 400;


void setup() {
  strip.begin();
  strip.show();
  
  Serial.begin(9600);
}

int value = 0;
int selected = -1;

void loop() {
  
  // reset selection
  if(selected > 0) {
    selected = -1;
  }
  
  // read all
  for(int i; i<5; i++) {
    values[i] = analogRead(sensors[i]);
    if(values[i] < THRESHOLD) {
      selected = i;
    }
  }
  
  if(selected == 0) {
    setPixel(0xFFFFFF);
  } else if(selected == 1) {
    setPixel(0xFF0000);
  } else if(selected == 2) {
    setPixel(0x00FF00);
  } else if(selected == 3) {
    setPixel(0x0000FF);
  } else if(selected == 4) {
    setPixel(0x99CC00);
  } else {
    setPixel(0x000000);
  }
  
  strip.show(); 
  delay(100);
  
  
  // debug
  
  Serial.print(values[0]);
  Serial.print("   ");
  Serial.print(values[1]);
  Serial.print("   ");
  Serial.print(values[2]);
  Serial.print("   ");
  Serial.print(values[3]);
  Serial.print("   ");
  Serial.print(values[4]);
  Serial.print("   ");
  Serial.print(selected);
  Serial.println("");
 
}


void setPixel(uint32_t color) {
strip.setPixelColor(0, color);
strip.setPixelColor(1, color);
}

void setPixel(uint8_t r, uint8_t g, uint8_t b) {
strip.setPixelColor(0, r, g, b);
strip.setPixelColor(1, r, g, b);
}

