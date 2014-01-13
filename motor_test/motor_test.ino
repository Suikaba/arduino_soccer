#include <Wire.h>
#include <DaisenMotorBoard.h>

daisen_motor_board motor_board;
int counter = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus
}

void loop() {
  char motor_stop[] = {0, 0, 0, 0};
  char go_front[] = {-80, -84, -80, -84};
  char go_back[] = {77, 88, 77, 87};
  char go_left[] = {80, -95, -80, 80};
  char go_right[] = {-65, 80, 53, -80};
  char go_left_front[] = {0, -80, -80, 0}; // ok 
  char go_right_front[] = {-74, 0, 0, -70}; //
  char go_left_back[] = {74, 0, 0, 70}; // 
  char go_right_back[] = {0, 80, 80, 0}; //
  if(counter != 7) {
    motor_board.set_motor_value(go_right);
    counter = (++counter)%8;
  } else {
    motor_board.set_motor_value(0, 0, 0, 0);
  }
  motor_board.move();
  
  delay(220);
}

