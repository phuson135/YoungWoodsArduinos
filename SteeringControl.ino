
// Author: Son Nguyen
// Gmail Address: phuson135@gmail.com
// Date: 03/08/23

// Motor Controller Used: http://www.icstation.com/motor-driver-board-module-speed-controller-regulator-module-p-12328.html
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>                // Optimized Encoder Library
#include <Wire.h>                   // I2C connection
#include <TM1637Display.h>          // Display for the Throttle and Brake Control.

// Define the pins connected to the calibration buttons
#define S_BUTTOM_L_PIN 4
#define S_BUTTON_RES_PIN 5 
#define S_BUTTON_R_PIN 6 
#define LED_PIN 9

// Define the connections for steering motor controllers
#define S_INB_PIN 8
#define S_INA_PIN 10
#define S_PWM_PIN 11

// Define the display connections
#define S_DISPLAY_IO_PIN 12
#define S_DISPLAY_CLK_PIN 13

#define GEAR_RATIO 1.5
#define NUM_REV 1.5
#define TICKS_PER_ROT 96
#define ALL_LEFT (int)(-NUM_REV*TICKS_PER_ROT*GEAR_RATIO)
#define ALL_RIGHT (int)(NUM_REV*TICKS_PER_ROT*GEAR_RATIO)
//130
#define MIN_SPEED 130
#define MIN_SPEED_EX1 230
#define MIN_SPEED_EX2 240
#define MAX_SPEED 255

#define I2C_ADDR 0x10

volatile int s_wanted_position;
volatile byte old_rcvData=0;
byte rcvData = 0;
int s_last_pos=0;
int calibrating=0; 
int right=0;
int left=0;
int s_current_position;

void DataRcv(int numBytes)
{
  while(Wire.available()) 
  { 
    rcvData = (int)(Wire.read());
    if (old_rcvData == rcvData){
      return;
    }else{
      s_wanted_position = map(rcvData, 0, 255, ALL_LEFT, ALL_RIGHT);
    }
    old_rcvData = rcvData;
  }
}

void DataRqst(){
  Wire.write(rcvData); // send address
}

// Create a display object of type TM1637Display
TM1637Display s_display = TM1637Display(S_DISPLAY_CLK_PIN, S_DISPLAY_IO_PIN);
Encoder s_encoder(2, 3);

// Motor Controller Used: http://www.icstation.com/motor-driver-board-module-speed-controller-regulator-module-p-12328.html


void setup() {
   // Set up the I2C connections
  Wire.begin(I2C_ADDR);
  
  // register an event handler for data receive and data request
  Wire.onReceive(DataRcv);
  Wire.onRequest(DataRqst);   
  
   // Set up the display
  s_display.setBrightness(5);
  s_display.clear();

   // Set up the pin Modes
  pinMode(S_PWM_PIN, OUTPUT);
  pinMode(S_INA_PIN, OUTPUT);
  pinMode(S_INB_PIN, OUTPUT);

  // Set them all to 0s
//analogWrite(S_PWM_PIN, 0); 
  digitalWrite(S_INA_PIN, LOW);
  digitalWrite(S_INB_PIN, LOW);

  // Set up the buttons
  pinMode(S_BUTTOM_L_PIN, INPUT);
  pinMode(S_BUTTON_R_PIN, INPUT);
  pinMode(S_BUTTON_RES_PIN, INPUT);
  
  s_encoder.write(0);

//  Serial.begin(9600);
}

void drive_left(int s_speed){
  digitalWrite(S_INA_PIN, HIGH);
  digitalWrite(S_INB_PIN, LOW);
  analogWrite(S_PWM_PIN, s_speed);
}

void drive_right(int s_speed){
  
  digitalWrite(S_INA_PIN, LOW);
  digitalWrite(S_INB_PIN, HIGH);
  analogWrite(S_PWM_PIN, s_speed);
}

void drive_stop(){
  digitalWrite(S_INA_PIN, LOW);
  digitalWrite(S_INB_PIN, LOW);
  analogWrite(S_PWM_PIN, 0);
}

void loop() { 
  
  s_current_position = s_encoder.read();
  //DISPLAY CURRENT POSITION
  Serial.println(s_current_position);
  if( abs(s_current_position-s_last_pos) > 1){
    s_display.clear();
    s_display.showNumberDec(s_current_position);
    
    s_last_pos = s_current_position;
  }

  
  if(calibrating==1){ 
    //Calibrating Procedure
    if(digitalRead(S_BUTTON_RES_PIN) == HIGH){
      s_encoder.write(0);
      calibrating=0;
      drive_stop(); 
      right = 0;
      left = 0;
    }else if(left == 1 && digitalRead(S_BUTTOM_L_PIN) == HIGH){
      drive_left(MIN_SPEED);
    }else if(right == 1 && digitalRead(S_BUTTON_R_PIN) == HIGH){
      drive_right(MIN_SPEED);
    }
  }else{ 
    if(digitalRead(S_BUTTON_R_PIN) == HIGH){
      right = 1;
      left = 0;
      calibrating=1;
    }else if(digitalRead(S_BUTTOM_L_PIN) == HIGH){
      right = 0;
      left = 1;
      calibrating=1;
    }else if (s_wanted_position > s_current_position + 4){
      if(abs(s_wanted_position-s_current_position)>10)
          drive_right(255);
      else if (s_wanted_position> ALL_RIGHT*1.2)
         drive_right(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED_EX2,MAX_SPEED));
      else if (s_wanted_position> ALL_RIGHT*1.2)
         drive_right(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED_EX1,MAX_SPEED));
      else
        drive_right(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED,MAX_SPEED));
    }else if (s_wanted_position < s_current_position - 4){
      if(abs(s_wanted_position-s_current_position)>10)
          drive_left(255);
      else if (s_wanted_position < -ALL_RIGHT*1.2)
        drive_left(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED_EX2,MAX_SPEED));
      else if (s_wanted_position < -ALL_RIGHT)
        drive_left(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED_EX1,MAX_SPEED));
      else
        drive_left(map(abs(s_wanted_position-s_current_position),0,ALL_RIGHT*2, MIN_SPEED,MAX_SPEED));
    }else{
      drive_stop();
    }
  }
//  Serial.println(rcvData);

//  analogWrite(LED_PIN, rcvData); // comment this out for the LED display.
}
