// Author: Son Nguyen
// Gmail Address: phuson135@gmail.com
// Date: 03/08/23

// Motor Controller Used: http://www.icstation.com/motor-driver-board-module-speed-controller-regulator-module-p-12328.html
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>                // Optimized Encoder Library
#include <Wire.h>                   // I2C connection
#include <TM1637Display.h>          // Display for the Throttle and Brake Control.

#define LED_PIN    9

// Define pins for the encoder
#define T_ENCODER_PIN_A 2
#define T_ENCODER_PIN_B 3

// Define the display connections
#define T_DISPLAY_CLK_PIN 5
#define T_DISPLAY_IO_PIN 4
#define B_DISPLAY_CLK_PIN 7
#define B_DISPLAY_IO_PIN 6

// Define the connections for braking and throttle motor controllers
#define T_PWM_PIN 10
#define T_IN1_PIN 1 // Moved from #9
#define T_IN2_PIN 8



#define B_PWM_PIN 11
#define B_IN3_PIN 12
#define B_IN4_PIN 13


// Define pin for the brake potentiometer and throttle encoder
#define B_ENCODER_PIN A0

// The I2C address of Throttle Brake Control Adruino Unit
#define I2C_ADDRESS 0x18

// Defining some constants for ease of usage
#define B_LOW 15               // Braking all the way
#define B_HIGH 40             // Not Braking

#define T_LOW 0               // Throttling at 0
#define T_HIGH 145*1.4        // Max Throttling.

// Create a display object of type TM1637Display
TM1637Display t_display = TM1637Display(T_DISPLAY_CLK_PIN, T_DISPLAY_IO_PIN);
TM1637Display b_display = TM1637Display(B_DISPLAY_CLK_PIN, B_DISPLAY_IO_PIN);

//Creating the encoder for throttle
Encoder t_encoder(T_ENCODER_PIN_A, T_ENCODER_PIN_B);

int t_current_position = 0;
int b_current_position = 0;
byte rcvData = 0;


volatile int t_wanted_position = 0;
volatile int b_wanted_position = B_LOW;
volatile byte old_rcvData=0;

int t_last_pos = 0;
int b_last_pos = 0;

void DataRcv(int numBytes)
{
  while(Wire.available()){ 
    rcvData = (int)(Wire.read());
    if (old_rcvData == rcvData){
      return;
    }else if(rcvData<128){
      b_wanted_position = map(rcvData, 0, 127, B_LOW , B_HIGH);
      t_wanted_position = 0;      // No throttle
    }else{
      t_wanted_position = map(rcvData-128, 0, 127, T_LOW , T_HIGH); 
      b_wanted_position = B_HIGH; // No braking
    }
    old_rcvData = rcvData;
  }
}

void DataRqst(){
  Wire.write(rcvData); // send address
}


void setup() {
  
  // Set up the I2C connections
  Wire.begin(I2C_ADDRESS);
  
  // register an event handler for data receive and data request
  Wire.onReceive(DataRcv);
  Wire.onRequest(DataRqst);   

  // Set up the display
  t_display.setBrightness(5);
  b_display.setBrightness(5);
  t_display.clear();
  b_display.clear();

  // Set up the pin Modes
  pinMode(T_PWM_PIN, OUTPUT);
  pinMode(T_IN1_PIN, OUTPUT);
  pinMode(T_IN2_PIN, OUTPUT);
  
  pinMode(B_PWM_PIN, OUTPUT);
  pinMode(B_IN3_PIN, OUTPUT);
  pinMode(B_IN4_PIN, OUTPUT);

  // Set them all to 0s
//  analogWrite(T_PWM_PIN, 0); 
//  analogWrite(B_PWM_PIN, 255); 
  digitalWrite(B_IN3_PIN, LOW);
  digitalWrite(B_IN4_PIN, LOW);
  digitalWrite(T_IN1_PIN, LOW);
  digitalWrite(T_IN2_PIN, LOW);

  // Set up the encoder to be at 0 position.
  t_encoder.write(0);
//  Serial.begin(9600);
}

void throttle_control(){
   t_current_position = t_encoder.read()/10;
  if( abs(t_current_position-t_last_pos) > 5){
    t_display.clear();
    t_display.showNumberDec(t_current_position);
    t_last_pos = t_current_position;
  }
  
  if (t_wanted_position > t_current_position+10){
    digitalWrite(T_IN1_PIN, LOW);
    digitalWrite(T_IN2_PIN, HIGH);
  }else if (t_wanted_position < t_current_position-10){
    digitalWrite(T_IN1_PIN, HIGH);
    digitalWrite(T_IN2_PIN, LOW);
  }else{
    digitalWrite(T_IN1_PIN, LOW);
    digitalWrite(T_IN2_PIN, LOW);
  }
  analogWrite(T_PWM_PIN, map(abs(t_wanted_position-t_current_position),T_LOW, T_HIGH, 30, 40)); 
}

void brake_control(){
  // BRAKE LINEAR ACTUATOR CONTROL
  b_current_position = map(analogRead(B_ENCODER_PIN),38, 990, 0, 100);
  if( abs(b_current_position-b_last_pos) > 8){
    b_display.clear();
    b_display.showNumberDec(b_current_position);
    b_last_pos = b_current_position;
  }
  
  if (b_wanted_position > b_current_position+4){
    digitalWrite(B_IN3_PIN, LOW);
    digitalWrite(B_IN4_PIN, HIGH);
  }else if (b_wanted_position < b_current_position-4){
    digitalWrite(B_IN3_PIN, HIGH);
    digitalWrite(B_IN4_PIN, LOW);
  }else{
    digitalWrite(B_IN3_PIN, LOW);
    digitalWrite(B_IN4_PIN, LOW);
  }
  
  int difference = abs(b_wanted_position-b_current_position);
  if(difference >= B_HIGH){
    analogWrite(B_PWM_PIN, 100);
  }else{
    analogWrite(B_PWM_PIN, map(difference,0, B_HIGH, 90, 100)); 
  }
}

void loop(){ 
  throttle_control();
  brake_control();
  // analogWrite(LED_PIN, rcvData); //Comment this out for LED display
  
}
