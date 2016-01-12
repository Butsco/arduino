#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


// set pin numbers:
const int bR = A4;
const int bG = A5;
const int bB = A7;
int stateR = 0;
int stateG = 0;
int stateB = 0;


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  pinMode(bR, INPUT);
pinMode(bG, INPUT);
pinMode(bB, INPUT);

Serial.begin(9600);
}

int mode = 0;
int frame = 0;

const int stepp = 10;
int R = 50;
int G = 0;
int B = 0;
uint32_t color = strip.Color(R,G,B);


void loop() {
  frame++;
  
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  //theaterChase(strip.Color(127, 127, 127), 50); // White
  //theaterChase(strip.Color(127,   0,   0), 50); // Red
  //theaterChase(strip.Color(  0,   0, 127), 50); // Blue
  
  if(mode==0) {
    
    int offset = (frame / 10) % 3; 
    for (int i=0; i < strip.numPixels(); i++) {
      if(i%3 == 0) {strip.setPixelColor(i+offset, color);}
      else {strip.setPixelColor(i+offset, 0);}
    }
    strip.show();
    
    
    /*
    int index = (frame / 10) % 12; 
    
    for(uint16_t i=0; i<strip.numPixels(); i++) {  
      if(i <= index) {strip.setPixelColor(i, color} 
      else {strip.setPixelColor(i, 0);}
    }
    
    strip.show();*/
    
    
    /*
   for(uint16_t i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + frame) & 255));
    }
    strip.show();
    */
  }
  else if(mode==1) {colorWipe(color, 50);}
  else if(mode==2) {rainbow(20);}

  //theaterChase(color, 10); // Blue
  /*
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(R,G,B));
  }
  strip.show();
 
  delay(50);*/
  
  
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
  
  stateR = digitalRead(bR);
  if (stateR == LOW) {R = (R+stepp) % 255;} 

  stateG = digitalRead(bG);
  if (stateG == LOW) {G = (G+stepp) % 255;} 

  stateB = analogRead(bB);
  if (stateB < 50) {B = (B+stepp) % 255;} 
  
  if(stateR == LOW && stateG == LOW) {
  mode = (mode+1) %3;
  }
  
  color = strip.Color(R,G,B);
  
  delay(10);

}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

