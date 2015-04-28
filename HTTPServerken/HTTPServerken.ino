#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10 // Use hardware SPI for the remaining pins // On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER); // you can change this clock speed
#define WLAN_SSID       "Vikingsigns"   // cannot be longer than 32 characters!
#define WLAN_PASS       "Vlaanderen"
#define WLAN_SECURITY   WLAN_SEC_WPA2 // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define LISTEN_PORT           80      // What TCP port to listen on for connections.// The HTTP protocol uses port 80 by default.
#define MAX_ACTION            10      // Maximum length of the HTTP action that can be parsed.
#define MAX_PATH              64      // Maximum length of the HTTP request path that can be parsed. // There isn't much memory available so keep this short!
#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20  // Size of buffer for incoming request data.
                                                          // Since only the first line is parsed this needs to be as large as the maximum action and path plus a little for whitespace and HTTP version.
#define TIMEOUT_MS            500    // Amount of time in milliseconds to wait for an incoming request to finish.  Don't set this
                                     // too high or your server could be slow to respond.

Adafruit_CC3000_Server httpServer(LISTEN_PORT);
uint8_t buffer[BUFFER_SIZE+1];
int bufindex = 0;
char action[MAX_ACTION+1];
char path[MAX_PATH+1];
#define PIN 20 // pin voor neopixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);  // Parameter 1 = number of pixels in strip, Parameter 2 = Arduino pin number (most are valid), Parameter 3 = pixel type flags
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
int red = 255;
int green = 255;
int blue = 255;




void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 

  
  // Initialise the module
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // Start listening for connections
  httpServer.begin();
  
  Serial.println(F("Listening for connections..."));

//START NEOPIXELS
//---------------
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println(F("stripping has begun"));
}

uint32_t c = strip.Color(255,255,255);
uint16_t i;

void loop(void)
{
  //theaterChase(strip.Color( red, green, blue), 50); // white
  
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();

  
// GET A CLIENT
//---------------
  Adafruit_CC3000_ClientRef client = httpServer.available(); // Try to get a client which is connected.
  if (client) {
    Serial.println(F("Client connected.")); 
    bufindex = 0; // Clear the incoming data buffer and point to the beginning of it.
    memset(&buffer, 0, sizeof(buffer));
    memset(&action, 0, sizeof(action)); // Clear action 
    memset(&path,   0, sizeof(path)); // Clear path strings
    
//READ CLIENT DATA
//-----------------
    unsigned long endtime = millis() + TIMEOUT_MS;// Set a timeout for reading all the incoming data.
    bool parsed = false; // Read all the incoming data until it can be parsed or the timeout expires.
    while (!parsed && (millis() < endtime) && (bufindex < BUFFER_SIZE)) {
      if (client.available()) {
        buffer[bufindex++] = client.read();
      }
      parsed = parseRequest(buffer, bufindex, action, path);
    }

// HANDLING the request if it was parsed.
//-----------------------------------------------
    if (parsed) {
      Serial.println(F("Processing request"));
      Serial.print(F("Action: ")); Serial.println(action);
      Serial.print(F("Path: ")); Serial.println(path);
      
      if (strcmp(action, "GET") == 0) { // Check the action to see if it was a GET request. strcmp = stringcompare
        if (strcmp(path, "/red") == 0) {
          Serial.println(F("confirm"));
          client.fastrprintln(F("HTTP/1.1 200 OK"));// First send the success response code.       
          client.fastrprintln(F("Content-Type: text/plain"));// Then send a few headers to identify the type of data returned and that the connection will not be held open.
          client.fastrprintln(F("Connection: close"));
          client.fastrprintln(F("Server: Adafruit CC3000"));
          // Send an empty line to signal start of body.
          client.fastrprintln(F(""));
          
          red = 255;
          green = 0;
          blue = 0;
          
          c = strip.Color(255,0,0);
          
          

          //theaterChase(strip.Color(red,green,blue), 50);
        }
       else if (strcmp(path, "/green") == 0) {
          Serial.println(F("confirm"));
          client.fastrprintln(F("HTTP/1.1 200 OK"));// First send the success response code.       
          client.fastrprintln(F("Content-Type: text/plain"));// Then send a few headers to identify the type of data returned and that the connection will not be held open.
          client.fastrprintln(F("Connection: close"));
          client.fastrprintln(F("Server: Adafruit CC3000"));
          // Send an empty line to signal start of body.
          client.fastrprintln(F(""));
          
          red = 0;
          green = 255;
          blue = 0;
          
          c = strip.Color(0,255,0);
          
          

          //theaterChase(strip.Color(red,green,blue), 50);
        }  else if (strcmp(path, "/off") == 0) {
          Serial.println(F("confirm"));
          client.fastrprintln(F("HTTP/1.1 200 OK"));// First send the success response code.       
          client.fastrprintln(F("Content-Type: text/plain"));// Then send a few headers to identify the type of data returned and that the connection will not be held open.
          client.fastrprintln(F("Connection: close"));
          client.fastrprintln(F("Server: Adafruit CC3000"));
          // Send an empty line to signal start of body.
          client.fastrprintln(F(""));
          
          red = 0;
          green = 255;
          blue = 0;
          
          c = strip.Color(0,0,0);
          
          

          //theaterChase(strip.Color(red,green,blue), 50);
        }  
        else if (strcmp(path, "/on") == 0) {
          Serial.println(F("confirm"));
          client.fastrprintln(F("HTTP/1.1 200 OK"));// First send the success response code.       
          client.fastrprintln(F("Content-Type: text/plain"));// Then send a few headers to identify the type of data returned and that the connection will not be held open.
          client.fastrprintln(F("Connection: close"));
          client.fastrprintln(F("Server: Adafruit CC3000"));
          // Send an empty line to signal start of body.
          client.fastrprintln(F(""));
          
          red = 0;
          green = 255;
          blue = 0;
          
          c = strip.Color(255,255,255);
          
          

          //theaterChase(strip.Color(red,green,blue), 50);
        }  
        //if (strcmp(path, "/green") == 0 {
        //  }
        else {     
          client.fastrprintln(F("HTTP/1.1 200 OK"));// First send the success response code.       
          client.fastrprintln(F("Content-Type: text/html"));// Then send a few headers to identify the type of data returned and that the connection will not be held open.
          client.fastrprintln(F("Connection: close"));
          client.fastrprintln(F("Server: Adafruit CC3000"));
          // Send an empty line to signal start of body.
          client.fastrprintln(F(""));
          client.fastrprintln(F("<html> <head>"));
          client.fastrprintln(F("<script>"));
          client.fastrprintln(F("x = new XMLHttpRequest();"));
          client.fastrprintln(F("function red() {x.open('GET','/red'); x.send(); console.log(x.status);}"));
          client.fastrprintln(F("</script> </head> <body>"));
          
          client.fastrprintln(F("<h1>Hello world!</h1>"));// Now send the response data.
          client.fastrprintln(F("<h1 onclick='red()'>RED</h1>"));
          client.fastrprint(F("You accessed path: ")); client.fastrprintln(path);
          client.fastrprintln(F("</body> </html>"));
          client.fastrprintln(F(""));

          
        }
        
      }
      else {
        // Unsupported action, respond with an HTTP 405 method not allowed error.
        client.fastrprintln(F("HTTP/1.1 405 Method Not Allowed"));
        client.fastrprintln(F(""));
      }
    }

    // Wait a short period to make sure the response had time to send before
    // the connection is closed (the CC3000 sends data asyncronously).
    //delay(100);

    // Close the connection when done.
    Serial.println(F("Client disconnected"));
    client.close();
  }
}


// Return true if the buffer contains an HTTP request.  Also returns the request
// path and action strings if the request was parsed.  This does not attempt to
// parse any HTTP headers because there really isn't enough memory to process
// them all.
// HTTP request looks like:
//  [method] [path] [version] \r\n
//  Header_key_1: Header_value_1 \r\n
//  ...
//  Header_key_n: Header_value_n \r\n
//  \r\n
bool parseRequest(uint8_t* buf, int bufSize, char* action, char* path) {
  // Check if the request ends with \r\n to signal end of first line.
  if (bufSize < 2)
    return false;
  if (buf[bufSize-2] == '\r' && buf[bufSize-1] == '\n') {
    parseFirstLine((char*)buf, action, path);
    return true;
  }
  return false;
}

// Parse the action and path from the first line of an HTTP request.
void parseFirstLine(char* line, char* action, char* path) {
  // Parse first word up to whitespace as action.
  char* lineaction = strtok(line, " ");
  if (lineaction != NULL)
    strncpy(action, lineaction, MAX_ACTION);
  // Parse second word up to whitespace as path.
  char* linepath = strtok(NULL, " ");
  if (linepath != NULL)
    strncpy(path, linepath, MAX_PATH);
}

// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
//------------------------------
// NEOPIXEL SHIT
//------------------------------
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
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
