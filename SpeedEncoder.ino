// Author: Son Nguyen
// Gmail Address: spn1@rice.edu
// Date: 03/08/23

// Motor Controller Used: http://www.icstation.com/motor-driver-board-module-speed-controller-regulator-module-p-12328.html
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>                // Optimized Encoder Library

// Define pins for the encoder
#define T_ENCODER_PIN_A 2
#define T_ENCODER_PIN_B 3


//Creating the encoder for throttle
Encoder t_encoder(T_ENCODER_PIN_A, T_ENCODER_PIN_B);

const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int acc=0;
int old_speed;
int new_speed;
int flag = 0;

void setup() {
  // Set up the encoder to be at 0 position.
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  old_speed=0;
  t_encoder.write(0);
  Serial.begin(9600);

}

void loop(){
  total -= readings[readIndex];
  readings[readIndex]=t_encoder.read();
  t_encoder.write(0);
  total += readings[readIndex];
  Serial.print("S");
  new_speed = (int)(total/numReadings);
  Serial.print(new_speed);
  Serial.print("|");
  Serial.println((int)(new_speed-old_speed));
  old_speed = new_speed;
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  delay(100);

}
