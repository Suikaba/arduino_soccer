/*far IR sensor test(IRM3638)*/
//結果；距離は検出できそう（5段階くらい？）

#include <Wire.h>

#define PIN_NUM 11

int time = 0;
int pin[PIN_NUM] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
// 0:none 1:gt 400 2:gt 500
int pin_value[PIN_NUM] = {0};

int find_max_pin() {
  int max_value = 0;
  int max_pin = 0;
  for(int i=0; i<PIN_NUM; ++i) {
    if(pin_value[i] > max_value) {
      max_value = pin_value[i];
      max_pin = i; // return value is actual pin number - 2
    }
  }
  return max_pin;
}

void request_event() {
  char max_pin = find_max_pin();
  Wire.write(max_pin);
}

void setup() {
  for(int i=0; i<PIN_NUM; ++i) {
    pinMode(pin[i],INPUT);
  }
  Serial.begin(9600);
  Wire.begin(0x10);
  Wire.onRequest(request_event);
}

void loop() {
  delay(100);
  for(int i=0; i<PIN_NUM; ++i) {
    pin_value[i] = pulseIn(pin[i], LOW, 1500);
    Serial.print(pin_value[i]); Serial.print(" ");
  }
  Serial.println();
}
