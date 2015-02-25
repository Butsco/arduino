const int R = 8;
const int G = 7;
const int B = 6;


void setup() {

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
}


void loop() {
  analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 255);
  delay(1000);
}
