// GND, wit
// DATA, groen, D2, D4
// CLOCK, geel, D3, RX
// VIN, rood 



#include "LPD8806.h"

// Number of RGB LEDs in strand:
int nLEDs = 50;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;


// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


const char* ssid = "RKM-Boven";
const char* password = "routerken123";

ESP8266WebServer server(80);

const int led = 13;

uint8_t  r = 0;
uint8_t  g = 0;
uint8_t  b = 0;

void handleRoot() {

  if(server.hasArg("r")) {r = server.arg("r").toInt();}
  if(server.hasArg("g")) {g = server.arg("g").toInt();}
  if(server.hasArg("b")) {b = server.arg("b").toInt();}


  String message = "<html><head><script>";
  message += "R=";
  message += r;
  message += "; G=";
  message += g;
  message += "; B=";
  message += b;
  message += ";";
  message += "function color(r,g,b) {x = new XMLHttpRequest(); x.open('GET', '/api?r='+r+'&g='+g+'&b='+b); x.send(); R=r; G=g; B=b;}";
  message += "</script></head><body style='background: rgb(";
  message += r;
  message += ", ";
  message += g;
  message += ", ";
  message += b;
  message += ")'>";
  message += "R: <button onclick='color(R+10, G, B)'>+</button><button onclick='color(R-10, G, B)'>-</button></h1>";
  message += "G: <button onclick='color(R, G+10, B)'>+</button><button onclick='color(R, G-10, B)'>-</button></h1>";
  message += "B: <button onclick='color(R, G, B+10)'>+</button><button onclick='color(R, G, B-10)'>-</button></h1>";
  message += "</body></html>";
  
  message += "\n";

  Serial.print("R: ");
  Serial.print(r);
  Serial.print("G: ");
  Serial.print(g);
  Serial.print("B: ");
  Serial.println(b);

  server.send(200, "text/html", message);
}

void handleApi() {

  if(server.hasArg("r")) {r = server.arg("r").toInt();}
  if(server.hasArg("g")) {g = server.arg("g").toInt();}
  if(server.hasArg("b")) {b = server.arg("b").toInt();}

  String message = "{\"r\": ";
  message += r;
  message += ", \"g\": ";
  message += g;
  message += ", \"b\": ";
  message += b;
  message += "}";

  Serial.print("R: ");
  Serial.print(r);
  Serial.print(" G: ");
  Serial.print(g);
  Serial.print(" B: ");
  Serial.println(b);

  server.send(200, "application/json", message);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/api", handleApi);
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");


  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}




//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}


// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void loop(void){
  server.handleClient();

//  rainbowCycle(0);

  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255,g,b));
  }

//  Serial.print("show");

  strip.setPixelColor(1, strip.Color(120,0,0));
  strip.setPixelColor(2, strip.Color(130,0,0));
  strip.show();
}


