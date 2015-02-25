const int R = 8;
const int G = 7;
const int B = 6;

const int pin = 12;

void setup() {
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
  pinMode(pin, INPUT);     
  
  color(0, 255, 0);
  
  Serial.begin(9600);
}

void loop(){
  // read the state of the pushbutton value:
  int button = analogRead(pin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (button == HIGH) {     
    color(0, 0, button);
    Serial.println("high");
  } 
  else {
    color(button, 0, 0); 
    Serial.println("low");
  }
}



void color(int r, int g, int b) {
        analogWrite(R, r);
        analogWrite(G, g);
        analogWrite(B, b);
}
