#include <Wire.h>

char motor_pow[4] = {0,0,0,0}; //for motor output
int counter = 0;
int motor_address = 0x14 >> 1;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus
}

#define MOTOR_NUM 4

void loop() {
  int motor_stop[MOTOR_NUM] = {0, 0, 0, 0};
  int go_front[MOTOR_NUM] = {-55, -50, -56, -50};
  int go_back[MOTOR_NUM] = {52, 50, 53, 50};
  int go_left[MOTOR_NUM] = {51, -50, -50, 50};
  int go_right[MOTOR_NUM] = {-51, 50, 50, -50};
  int go_left_front[MOTOR_NUM] = {0, -50, -50, 0}; 
  int go_right_front[MOTOR_NUM] = {-55, 0, 0, -52};
  int go_left_back[MOTOR_NUM] = {50, 0, 0, 55};
  int go_right_back[MOTOR_NUM] = {0, 50, 55, 0};
  set_motor_value(51, -50, -70, 70);
  move();
  counter = (++counter)%8;
  delay(500);
}

// Daisen 6ch(4 motor control board
// input val:-100~100 (0:stop, 999:brakes)
void set_motor_value(char val1, char val2, char val3, char val4)
{
  char vals[4] = {val1, val2, val3, val4};
  for(int i=0; i<4; ++i)
  {
    if(vals[i] < 0 && vals[i] >= -100){
      vals[i] = abs(vals[i]);
      motor_pow[i] = vals[i] + 128;
    } else if(vals[i] >= 0 && vals[i] <= 100) {
      motor_pow[i] = vals[i];
    } else if(vals[i] == 999) {
      motor_pow[i] = 128;
    } else {
      return;
    }
  }
}

void move(){
  Wire.beginTransmission(0x14>>1);
  Wire.write(motor_pow[0]);
  Wire.write(motor_pow[1]);
  Wire.write(motor_pow[2]);
  Wire.write(motor_pow[3]);
  Wire.write(0); // dummy
  Wire.write(0); // dummy
  Wire.endTransmission();
}
