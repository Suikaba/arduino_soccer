// far IR sensor test(IRM3638)
// 結果；距離は検出できそう（5段階くらい？）

#include <Wire.h>

#define PIN_NUM 11

int far_ir_pin[PIN_NUM] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
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

void get_next_pin(int pin, int* ret) {
  if(pin == 2) {
    ret[0] = 3; ret[1] = 4;
  } else if(pin == 3) {
    ret[0] = 2; ret[1] = 5;
  } else if(pin == 11) {
    ret[0] = 9; ret[1] = 12;
  } else if(pin == 12) {
    ret[0] = 10; ret[1] = 11;
  } else {
    ret[0] = pin-2; ret[1] = pin+2;
  }
}

//memo
// 0:,300)
// 1:[300,350)
// 2:[350,400)
// 3:[400,450)
// 4:[450,500)
// 5:[500,540)
// 6:[540,
void request_event() {
  int max_pin = find_max_pin();
  byte send_value = max_pin*10;
  if(pin_value[max_pin] > 540) {
    send_value += 6;
  } else if(pin_value[max_pin] > 500) {
    send_value += 5;
  } else if(pin_value[max_pin] > 450) {
    send_value += 4;
  } else if(pin_value[max_pin] > 400) {
    send_value += 3;
  } else if(pin_value[max_pin] > 350) {
    send_value += 2;
  } else if(pin_value[max_pin] > 300) {
    send_value += 1;
  } //else {
    //send_value += 0;
  //}
  Wire.write(send_value);
}

void setup() {
  for(int i=0; i<PIN_NUM; ++i) {
    pinMode(far_ir_pin[i],INPUT);
  }
  Wire.begin(0x10);
  Wire.onRequest(request_event);
}

void loop() {
  delay(20);
  for(int i=0; i<PIN_NUM; ++i) {
    pin_value[i] = pulseIn(far_ir_pin[i], LOW, 1500);
  }
}
