const int buttonPin = A0;

int buttonState = 0;

void setup() {
  pinMode(buttonPin, INPUT);
 
  Serial.begin(9600); 
}

int state = HIGH;
int button = 0;

void loop(){
  button = digitalRead(buttonPin);
  
  if(button != state) {
    state = button;
    if (button == HIGH) {     
      Serial.println("1"); 
    } 
    else {
      Serial.println("0"); 
    }
  }
  
  delay(100);
}
