const int START = 7;
const int RESET = 6;
const int RED = 5;
const int BLU = 4;

const int pins[] = {START, RESET, RED, BLU};

int state[] = {LOW, LOW, LOW, LOW};
int last[] = {LOW, LOW, LOW, LOW};
long time[] = {0,0,0,0};
long debounce = 50;

void setup() {  
  for(int i=0; i<4; i++) pinMode(pins[i], INPUT);      
  
  Serial.begin(9600);
}

int reading;

void loop() {
  //Serial.println("hello"); 
  
  for(int i=0; i<4; i++) {
    reading = digitalRead(pins[i]);
    
    if (reading != last[i]) {
      // reset the debouncing timer
      time[i] = millis();
    }
    
    if ((millis() - time[i]) > debounce) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
  
      // if the button state has changed:
      if (reading != state[i]) {
        state[i] = reading;
  
        // only toggle the LED if the new button state is HIGH
        if (reading == HIGH) {
          button(pins[i]);
        }
      }
    }
    
    last[i] = reading;
    
  }

}


// GAME LOGIC



boolean playing = false;
unsigned int score_red = 0;
unsigned int score_blu = 0;
unsigned int goals[100];

const unsigned int WIN_GOALS = 3;
const unsigned int WIN_DIFF = 2;

void button(int button) {

  if(button == START) {
    Serial.println("===   START   ===");
    
    // reset everything
    playing = true;

    score_red = 0;    
    score_blu = 0;
    
    for(int i=0; i<100; i++) {
       goals[i] = 0;  
    }


    
  } else if(playing && button == RESET) {
    Serial.println("MIDDLE!");
    
    // position in goals array
    int pointer = score_red + score_blu;

    if(pointer > 0) {
      int last = goals[pointer-1];
      
      Serial.print("Cancelling last goal:");
      if(last == RED) {
        score_red--;
        Serial.println(" red");
      } else if(last == BLU) {
        score_blu--;
        Serial.println(" blu");
      }  
      
      goals[pointer-1] = 0;      
      
    }

    scores();

     
   
    
  } else if(playing && button == RED) {
    Serial.println("RED SCORED!"); 
    
    score(RED);
    
    
  } else if(playing && button == BLU) {
    Serial.println("BLU SCORED!"); 
    
    score(BLU);
  }

}

void score(int team) {
  
  // position in goals array
  int pointer = score_red + score_blu;
  
  if(team == RED) {
    score_red++;
    goals[pointer] = RED;
    
    // check win
    if(score_red >= WIN_GOALS && score_red >= score_blu +WIN_DIFF) {
      Serial.println("RED WINS!!"); 
      playing = false;
    }    
    
  } else if(team == BLU) {
    score_blu++;
    goals[pointer] = BLU;
    
    // check win
    if(score_blu >= WIN_GOALS && score_blu >= score_red +WIN_DIFF) {
      Serial.println("BLU WINS!!"); 
      playing = false;
    } 
    
  }
  
  scores();
  
}

void scores() {
    // position in goals array
    int pointer = score_red + score_blu;

    // RED x vs x BLU
    Serial.print("RED "); 
    Serial.print(score_red);
    Serial.print(" vs "); 
    Serial.print(score_blu);
    Serial.println(" BLU"); 
    
    for(int i=0; i<pointer; i++) {
      if(i+1<10) {
        Serial.print("  ");
      } else {
        Serial.print(" ");
      }
      Serial.print(i+1);
      if(goals[i] == RED) {
        Serial.println(" red");
      } else if(goals[i] == BLU) {
        Serial.println(" blu");
      } 
    
    }
}





