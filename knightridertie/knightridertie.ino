#include <Adafruit_NeoPixel.h>

// SETUP YOUR OUTPUT PIN AND NUMBER OF PIXELS
#define PIN 6
#define NUM_PIXELS  12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int speed = 60;
int bar = 5;
int offset = bar/2;
void setup() {
  pixels.begin();
  delay(1000);
}

void loop() {
    for(int i = -offset; i<=NUM_PIXELS-bar+offset; i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      for(int j = 0; j-i<NUM_PIXELS;j++){
        if (j>=i && j-i<bar){          
         pixels.setPixelColor(j, pixels.Color(200*(j-i+1)/bar,0,0)); // Moderately bright green color.         
        }
        else{
          pixels.setPixelColor(j,pixels.Color(0,0,0));
        }
        pixels.show(); // This sends the updated pixel color to the hardware.
       }
    
        delay(speed); // Delay for a period of time (in milliseconds).
    }
    for(int i = -offset; i<=NUM_PIXELS-bar+offset; i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      for(int j = 0; j-i<NUM_PIXELS;j++){
        if (j>=i && j-i<bar){          
         pixels.setPixelColor(NUM_PIXELS-j-1, pixels.Color(200*(j-i+1)/bar,0,0)); // Moderately bright green color.         
        }
        else{
          pixels.setPixelColor(NUM_PIXELS-j-1,pixels.Color(0,0,0));
        }
        pixels.show(); // This sends the updated pixel color to the hardware.
       }
        delay(speed); // Delay for a period of time (in milliseconds).   
    }

}
