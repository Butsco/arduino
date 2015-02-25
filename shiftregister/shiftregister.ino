const int CLOCK = 13; 
const int DATA_IN = 12; 

int buttonState = 0; 

void setup() {
  pinMode(CLOCK, OUTPUT);      
  pinMode(DATA_IN, OUTPUT);
  
  digitalWrite(CLOCK, LOW);  
  digitalWrite(DATA_IN, LOW); 
  
  //Serial.begin(9600);
}

int led = 0;
int frame = 0;

void loop(){
  //Serial.print("frame ");
  //Serial.print(frame);
  //Serial.print("  led ");
  //Serial.println(led);
  
  if (led < 8) {   
    
    //Serial.print("check [[");
    //Serial.println(led==frame);
    
    digitalWrite(DATA_IN, led==frame ? LOW:HIGH);  
    digitalWrite(CLOCK, HIGH);

    digitalWrite(CLOCK, LOW);
 
    led++;  
  } 
  
  
  if(led == 8) {
    led = 0;
    frame++;
    frame = frame%8;
  
    delay(1000);
  }
  
  
}
