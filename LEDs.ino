#include <Adafruit_NeoPixel.h>

#define PIN_THROTTLE  11
#define PIN_STEER    3
#define N_LEDS 144
#define THROT_IN A0
#define STEER_IN A5

Adafruit_NeoPixel strip_throttle = Adafruit_NeoPixel(N_LEDS, PIN_THROTTLE, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_steer = Adafruit_NeoPixel(N_LEDS, PIN_STEER, NEO_GRB + NEO_KHZ800);

uint32_t blue = strip_throttle.Color(0, 0, 255);
uint32_t red = strip_throttle.Color(255, 0, 0);
uint32_t green = strip_throttle.Color(0, 255, 0);

int throttle = 128;
int steering = 128;

void setup() {
  strip_throttle.begin();
  strip_steer.begin();
  
  Serial.begin(9600);
}

void loop() {
  int throttle_val = map(analogRead(THROT_IN), 0, 906, 0, 255);
  int steer_val = map(analogRead(STEER_IN), 0, 906, 0, 255);
  
  chase_steering(floor(throttle_val * .235)); //
  
  chase_throttle(floor(steer_val * .235)); //
  Serial.println(throttle_val);
//  Serial.println(steer_val);
}

static void chase_steering(int position) {
    strip_throttle.fill();
    if(position < 30){
      strip_throttle.fill(blue, 30, 30 - position);
    }
    if(position > 30){
      strip_throttle.fill(blue, 60 - position, position - 30);
    }
    strip_throttle.show();
}

static void chase_throttle(int position) {
    strip_steer.fill();
    if(position < 30){
      strip_steer.fill(red, 30, 30 - position);
    }
    if(position > 30){
      strip_steer.fill(green, 60 - position, position - 30);
    }
    strip_steer.show();
}
