#include<Wire.h>


//int LEDPin[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int LEDPin[] = {21,20,19,18,17,16,15,14,8,9,10,11,12,13,7,6};
int LayerPin[] = {A0, A1, A2, A3};



const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

  
void setup(){
  
  // setup fancy chipke
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  
  
  // opens serial port, sets data rate to 9600 bps
  Serial.begin(9600);
  
  // Set up LED columns as output
  for(int pin = 0; pin < 16 ; pin++ ){
    pinMode(LEDPin[pin],OUTPUT);
    digitalWrite(LEDPin[pin],LOW);
  }
  // Set up LED Layers as output
  for(int layer = 0; layer < 4; layer++){
    pinMode(LayerPin[layer], OUTPUT);
    digitalWrite(LayerPin[layer], HIGH);
  }

}

boolean leds[4][16] = {
{
false,false,false,false,
false,false,false,false,
false,false,false,false,
false,false,false,false
},{
false,false,false,false,
false,false,false,false,
false,false,false,false,
false,false,false,false
},{
false,false,false,false,
false,false,false,false,
false,false,false,false,
false,false,false,false
},{
false,false,false,false,
false,false,true,false,
false,false,false,false,
false,false,false,false
}
};

void setLED(int led, boolean state) {
  leds[led/16][led%16] = state;
}

int frame = 0;

void loop() {
  
  fancyChip();
  return;
  
  int index = 0;
  
  for(int i=0; i<64; i++) {
  for(int j=0; j<64; j++) {
  
    setLED(j, i==j);
    
  }
  showFrame();
  delay(5);
  }
  
  
  /*
  for(int layer = 0; layer < 4; layer++) {
    
      for(int led = 0; led < 16; led++) {  
      
       leds[layer][led] = frame==layer;
        
      }
   }
   
  frame = (frame+1) % 4;
  
  showFrame();
  delay(100);
  */
  
}


void fancyChip() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  delay(333);

}




void showFrame() {
  
   // display the leds data structure layer by layer
  for(int layer = 0; layer < 4; layer++) {
    
      for(int led = 0; led < 16; led++) {
          if(leds[layer][led]) {
              digitalWrite(LEDPin[led],HIGH);
          } else {
              digitalWrite(LEDPin[led],LOW);
          }
      }
   
      digitalWrite(LayerPin[layer],LOW);
      delay(4); 
      digitalWrite(LayerPin[layer],HIGH);
  }
  
}
