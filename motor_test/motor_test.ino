#include <Wire.h>
#include <DaisenMotorBoard.h>

DaisenMotorBoard motor_board;
int counter = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus
}

void loop() {
  // 電池の充電がちゃんと出来てる時
  int motor_stop[] = {0, 0, 0, 0};
  int go_front[] = {-80, -81, -80, -81};
  int go_back[] = {80, 81, 80, 81};
  int go_left[] = {80, -80, -83, 83};
  int go_right[] = {-80, 80, 83, -83};
  int go_left_front[] = {0, -75, -75, 0}; // ok 
  int go_right_front[] = {-74, 0, 0, -70}; //
  int go_left_back[] = {74, 0, 0, 70}; // 
  int go_right_back[] = {0, 75, 75, 0}; //
  go_front[0]=-50; go_front[1]=-51; go_front[2]=-50; go_front[3]=-51;
        go_back[0]=50; go_back[1]=51; go_back[2]=50; go_back[3]=51;
        go_left[0]=49; go_left[1]=-49; go_left[2]=-50; go_left[3]=50;
        go_right[0]=-51; go_right[1]=50; go_right[2]=50; go_right[3]=-50;
        go_left_front[0]=0; go_left_front[1]=-50; go_left_front[2]=-50; go_left_front[3]=0;
        go_right_front[0]=-55; go_right_front[1]=0; go_right_front[2]=0; go_right_front[3]=-52;
        go_left_back[0]=50; go_left_back[1]=0; go_left_back[2]=0; go_left_back[3]=55;
        go_right_back[0]=0; go_right_back[1]=50; go_right_back[2]=55; go_right_back[3]=0;
  if(counter != 7) {
    motor_board.set_motor_value(go_left);
    counter = (++counter)%8;
  } else {
    motor_board.set_motor_value(0, 0, 0, 0);
  }
  motor_board.move();
  
  delay(220);
}

