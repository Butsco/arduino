
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <CC3000_MDNS.h>

// CC3000 configuration
#define     ADAFRUIT_CC3000_IRQ    3    // MUST be an interrupt pin!
#define     ADAFRUIT_CC3000_VBAT   5    // VBAT and CS can be any two pins
#define     ADAFRUIT_CC3000_CS     10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, 
                                         ADAFRUIT_CC3000_IRQ, 
                                         ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);

// Wireless network configuration
#define     WLAN_SSID              "myNetwork"      // cannot be longer than 32 characters!
#define     WLAN_PASS              "myPassword"
#define     WLAN_SECURITY          WLAN_SEC_WPA2  // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

// Neo pixel configuration
#define     PIXEL_PIN              7    // The pin which is connected to the neo pixel strip input.
#define     PIXEL_COUNT            90   // The number of neo pixels in the strip.

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Create a server to listen on port 80 (HTTP).
Adafruit_CC3000_Server server(80);

// Create a multicast DNS responder.
MDNSResponder mdns;

// Color scheme definitions.
struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  
  Color(uint8_t red, uint8_t green, uint8_t blue): red(red), green(green), blue(blue) {}
  Color(): red(0), green(0), blue(0) {}
};

struct ColorScheme {
  Color* colors;
  uint8_t count;
 
  ColorScheme(Color* colors, uint8_t count): colors(colors), count(count) {} 
};

Color rgbColors[3] = { Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255) };
ColorScheme rgb(rgbColors, 3);

Color christmasColors[2] = { Color(255, 0, 0), Color(0, 255, 0) };
ColorScheme christmas(christmasColors, 2);

Color hanukkahColors[2] = { Color(0, 0, 255), Color(255, 255, 255) };
ColorScheme hanukkah(hanukkahColors, 2);

Color kwanzaaColors[3] = { Color(255, 0, 0), Color(0, 0, 0), Color(0, 255, 0) };
ColorScheme kwanzaa(kwanzaaColors, 3);

Color rainbowColors[7] = { Color(255, 0, 0), Color(255, 128, 0), Color(255, 255, 0), Color(0, 255, 0), Color(0, 0, 255), Color(128, 0, 255), Color(255, 0, 255) };
ColorScheme rainbow(rainbowColors, 7);

Color incandescentColors[2] = { Color(255, 140, 20), Color(0, 0, 0) };
ColorScheme incandescent(incandescentColors, 2);

Color fireColors[3] = { Color(255, 0, 0), Color(255, 102, 0), Color(255, 192, 0) };
ColorScheme fire(fireColors, 3);

ColorScheme schemes[7] = { incandescent, rgb, christmas, hanukkah, kwanzaa, rainbow, fire };

// Enumeration of possible pattern types.
enum Pattern { BARS = 0, GRADIENT };

// Bar width values (in number of pixels/lights) for different size options.
int barWidthValues[3] = { 1,      // Small
                          3,      // Medium
                          6  };   // Large

// Gradient width values (in number of gradient repetitions, i.e. more repetitions equals a smaller gradient) for different size options.
int gradientWidthValues[3] = { 12,     // Small
                               6,      // Medium
                               2   };  // Large

// Speed values in amount of milliseconds to move one pixel/light.  Zero means no movement.
int speedValues[4] = { 0,       // None
                       500,     // Slow
                       250,     // Medium
                       50   };  // Fast

// Variables to hold current state.
int currentScheme = 0;
Pattern currentPattern = BARS;
int currentWidth = 0;
int currentSpeed = 0;
String buffer;

void setup() {
  Serial.begin(115200);
  
  // Reserve space in the buffer string so there isn't dynamic memory allocation in the loop.
  buffer.reserve(15);
  
  // Connect to the wifi network.
  Serial.println(F("Hello, CC3000!\n")); 
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  // Initialize the neo pixel strip.
  strip.begin();
  strip.show();

  // Start multicast DNS responder on 'arduino.local' address.
  if (!mdns.begin("arduino", cc3000)) {
    Serial.println(F("Error setting up MDNS responder!"));
    while(1); 
  }

  // Start listening for HTTP connections.
  server.begin();
  
  Serial.println(F("Listening for connections..."));  
}

// Compute the color of a pixel at position i using a gradient of the color scheme.  
// This function is used internally by the gradient function.
struct Color gradientColor(struct ColorScheme& scheme, int range, int gradRange, int i) {
  int curRange = i / range;
  int rangeIndex = i % range;
  int colorIndex = rangeIndex / gradRange;
  int start = colorIndex;
  int end = colorIndex+1;
  if (curRange % 2 != 0) {
    start = (scheme.count-1) - start;
    end = (scheme.count-1) - end;
  }
  return  Color(map(rangeIndex % gradRange, 0, gradRange, scheme.colors[start].red,   scheme.colors[end].red),
                map(rangeIndex % gradRange, 0, gradRange, scheme.colors[start].green, scheme.colors[end].green),
                map(rangeIndex % gradRange, 0, gradRange, scheme.colors[start].blue,  scheme.colors[end].blue)); 
}

// Display a gradient of colors for the provided color scheme.
// Repeat is the number of repetitions of the gradient (pick a multiple of 2 for smooth looping of the gradient).
// SpeedMS is the number of milliseconds it takes for the gradient to move one pixel.  Set to zero for no animation.
void gradient(struct ColorScheme& scheme, int repeat = 1, int speedMS = 1000) {   
  if (scheme.count < 2) return;
  
  int range = (int)ceil((float)PIXEL_COUNT / (float)repeat);
  int gradRange = (int)ceil((float)range / (float)(scheme.count - 1));
  
  unsigned long time = millis();
  int offset = speedMS > 0 ? time / speedMS : 0;

  Color oldColor = gradientColor(scheme, range, gradRange, PIXEL_COUNT-1+offset);  
  for (int i = 0; i < PIXEL_COUNT; ++i) {
    Color currentColor = gradientColor(scheme, range, gradRange, i+offset);
    if (speedMS > 0) {
      // Blend old and current color based on time for smooth movement.
      strip.setPixelColor(i, map(time % speedMS, 0, speedMS, oldColor.red,   currentColor.red),
                             map(time % speedMS, 0, speedMS, oldColor.green, currentColor.green),
                             map(time % speedMS, 0, speedMS, oldColor.blue,  currentColor.blue));
    }
    else {
      // No animation, just use the current color. 
      strip.setPixelColor(i, currentColor.red, currentColor.green, currentColor.blue);
    }
    oldColor = currentColor; 
  }
  strip.show(); 
}

// Display solid bars of color for the provided color scheme.
// Width is the width of each bar in pixels/lights.
// SpeedMS is number of milliseconds it takes for the bars to move one pixel.  Set to zero for no animation.
void bars(struct ColorScheme& scheme, int width = 1, int speedMS = 1000) {
  int maxSize = PIXEL_COUNT / scheme.count;
  if (width > maxSize) return;
  
  int offset = speedMS > 0 ? millis() / speedMS : 0;
  
  for (int i = 0; i < PIXEL_COUNT; ++i) {
    int colorIndex = ((i + offset) % (scheme.count * width)) / width;
    strip.setPixelColor(i, scheme.colors[colorIndex].red, scheme.colors[colorIndex].green, scheme.colors[colorIndex].blue);
  }
  strip.show(); 
}

void loop() {
  // Update MDNS responder.
  mdns.update();
    
  // Handle any HTTP connections.
  Adafruit_CC3000_ClientRef client = server.available();
  if (client) {
    buffer = "";
    // Expect "GET /arduino/".
    while (client.available() && buffer.length() < 13) {
      buffer = buffer + char(client.read());
    }
    if (buffer != "GET /arduino/" || !client.available()) {
      // Return error
      client.close();
      return;
    }
    // Parse value until '/' character.
    buffer = "";
    char ch = client.read();
    while (client.available() && ch != '/') {
      buffer = buffer + ch;
      ch = client.read();
    }
    if (ch != '/' || !client.available()) {
      client.close();
      return; 
    }
    // Parse a single digit value.
    int value = client.read() - '0';
    // Update appropriate state for the associated command and value.
    if (buffer == "scheme") {
      currentScheme = constrain(value, 0, 6);
    }
    else if (buffer == "pattern") {
      currentPattern = (Pattern)constrain(value, 0, 1);
    }
    else if (buffer == "width") {
      currentWidth = constrain(value, 0, 2);
    }
    else if (buffer == "speed") {
      currentSpeed = speedValues[constrain(value, 0, 3)];
    }
    // Return an empty JSON response since the web page is technically expecting JSON data.
    client.fastrprint(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
    // Close the connection.
    client.close();
  }
  
  // Update pixels based on current state.
  if (currentPattern == BARS) {
    bars(schemes[currentScheme], barWidthValues[currentWidth], currentSpeed);
  }
  else if (currentPattern == GRADIENT) {
    gradient(schemes[currentScheme], gradientWidthValues[currentWidth], currentSpeed);
  }
}

