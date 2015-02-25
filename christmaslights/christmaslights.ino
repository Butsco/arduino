const int LED = 9; 
const int SENSOR = A5;


void setup() {
  pinMode(LED, OUTPUT);      
  
  digitalWrite(LED, HIGH);  
  
  Serial.begin(9600);
}

int led = 0;
int frame = 0;

int value = 0;
void loop(){
  
  
  value = analogRead(SENSOR);   
  
  //Serial.print("frame ");
  //Serial.print(frame);
  //Serial.print("  led ");
  Serial.println(value);
  /*
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
  }*/
  
  
}
