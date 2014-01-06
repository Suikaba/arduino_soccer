/*soccer main program*/

#include <Wire.h>

//#define SPEED1

#define MOTOR_NUM    4
#define PING_PIN_NUM 4
#define NEAR_IR_NUM  8

// =======================================================================
// port
// =======================================================================
#define SWITCH1    48
#define SWITCH2    50
#define LED1       49
#define LED2       51
#define LED3       53
int near_ir_pin[NEAR_IR_NUM] = {0, 1, 2, 3, 4, 5, 6, 7};

// =======================================================================
// I2C address
// =======================================================================
#define FAR_IR_ADDRESS   0x10
#define PING_ADDRESS     0x16
#define MOTOR_ADDRESS    (0x14>>1)
#define COMPASS_ADDRESS  0x1E
//#define COMPASS_ADDRESS  (0x42>>1)

// =======================================================================
// prototype declaration
// =======================================================================
void set_motor_power(char vals[MOTOR_NUM]);
void set_motor_power(char val1, char val2, char val3, char val4);
int adjust_direction();

// =======================================================================
// variables
// =======================================================================
byte motor_pow[MOTOR_NUM] = {0};
int motor_status[MOTOR_NUM] = {0, 0, 0, 0}; // 0:stop, 1:front 2:back
int ping_value[PING_PIN_NUM] = {0};
unsigned long near_ir_value[NEAR_IR_NUM] = {0};
int compass_reference = 0;
int compass_value = 0;
int far_ir_num = -1;
int far_ir_status = -1;
int counter=0;
int motor_counter=0;


// =======================================================================
// setup
// =======================================================================
void setup(){
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  
  delay(100);
  int compass_sum = 0;
  for(int i=0; i<3; ++i) {
    compass_sum += compass_read();
  }
  compass_reference = compass_sum / 3;
}

// =======================================================================
// main loop
// =======================================================================
void loop(){
#ifdef SPEED1 // 柔らか?マット
  char go_back[MOTOR_NUM] = {70, 70, 70, 70};
  char go_straight[MOTOR_NUM] = {-70, -60, -70, -60};
#else // 硬いほう（すべるほう）
  int motor_stop[MOTOR_NUM] = {0, 0, 0, 0};
  int go_front[MOTOR_NUM] = {-55, -50, -56, -50};
  int go_back[MOTOR_NUM] = {52, 50, 53, 50};
  int go_left[MOTOR_NUM] = {51, -50, -50, 50};
  int go_right[MOTOR_NUM] = {-51, 50, 50, -50};
  int go_left_front[MOTOR_NUM] = {0, -50, -50, 0}; 
  int go_right_front[MOTOR_NUM] = {-55, 0, 0, -52};
  int go_left_back[MOTOR_NUM] = {50, 0, 0, 55};
  int go_right_back[MOTOR_NUM] = {0, 50, 55, 0};
#endif

  int founded_ball = 0;
  int nearest_ir[2] = {};
  const int width = 150;
  const int from_reference = compass_from_reference();
  //Serial.println(from_reference);
  read_far_ir();
  read_ping();

  if(far_ir_status == 0) {
    if(adjust_direction(from_reference) == 0) {
      if(ping_value[1] > 50) {
        int ping_sub = ping_value[2] - ping_value[3];
        if(ping_sub < -20) {
          set_motor_value(0, 40, 42, 0);
        } else if(ping_sub > 20) {
          set_motor_value(40, 0, 0, 42);
        } else {
          set_motor_value(41, 40, 42, 40);
        }
      } else if(ping_value[1] < 12) {
        int ping_sub = ping_value[2] - ping_value[3];
        if(ping_sub < -20) {
          set_motor_value(0, -40, -42, 0);
        } else if(ping_sub > 20) {
          set_motor_value(-40, 0, 0, -42);
        } else {
          set_motor_value(motor_stop);
        }
      } else {
        int ping_sub = ping_value[2] - ping_value[3];
        if(ping_sub < -20) {
          set_motor_value(-40, 40, 40, -40);
        } else if(ping_sub > 20) {
          set_motor_value(40, -40, -40, 40);
        } else {
          set_motor_value(motor_stop);
        }
      }
    }
  } else if(far_ir_status >= 5) {
    //todo
    //read_near_ir(nearest_ir);
    Serial.println("far_ir_status>=5");
    set_motor_value(0, 0, 0, 0);
    //founded_ball = 1;
  } else {
    if(abs(from_reference) > 900) {
      adjust_direction();
    } else {
      //founded_ball = 1;
      const int offset = far_ir_status;
      int compass_offset = 0;
      if(from_reference < -width || from_reference > width) {
        compass_offset = (from_reference/75); 
      }
      switch(far_ir_num) {
        case 0:
        case 1:
        case 2:
          // go_front
          set_motor_value(-55+offset-compass_offset,
                          -50+offset+compass_offset,
                          -56+offset-compass_offset,
                          -50+offset+compass_offset);
                          Serial.print("front ");
          output_motor_power(-55+offset-compass_offset,
                          -50+offset+compass_offset,
                          -56+offset-compass_offset,
                          -50+offset+compass_offset);
          break;
        case 3:
          // go_left_front
          set_motor_value(0,
                          -50+offset+compass_offset,
                          -50+offset-compass_offset,
                          0);
                          Serial.print("left_front ");
            output_motor_power(0,
                          -50+offset+compass_offset,
                          -50+offset-compass_offset,
                          0);
          break;
        case 4:
          // go_right_front
          set_motor_value(-55+offset-compass_offset,
                          0,
                          0,
                          -52+offset+compass_offset);
                          Serial.print("right_front ");
          output_motor_power(-55+offset-compass_offset,
                          0,
                          0,
                          -52+offset+compass_offset);
          break;
        case 5:
          // go_left
          set_motor_value(51-offset-compass_offset,
                          -50+offset+compass_offset,
                          -50+offset-compass_offset,
                          50-offset+compass_offset);
                          Serial.print("left ");
          output_motor_power(51-offset-compass_offset,
                          -50+offset+compass_offset,
                          -50+offset-compass_offset,
                          50-offset+compass_offset);
          break;
        case 6:
          // go_right
          //Serial.println(compass_offset);
          set_motor_value(-51+offset-compass_offset,
                          50-offset+compass_offset,
                          50-offset-compass_offset,
                          -50+offset+compass_offset);
                          Serial.print("right ");
          output_motor_power(-51+offset-compass_offset,
                          50-offset+compass_offset,
                          50-offset-compass_offset,
                          -50+offset+compass_offset);
          break;
        case 7:
          // go_left_back
          set_motor_value(50-offset-compass_offset,
                          0,
                          0,
                          55-offset+compass_offset);
                          Serial.print("left_back ");
          output_motor_power(50-offset-compass_offset,
                          0,
                          0,
                          55-offset+compass_offset);
          break;
        case 8:
          // go_right_back
          set_motor_value(0,
                          50-offset+compass_offset,
                          55-offset-compass_offset,
                          0);
                          Serial.print("right_back ");
          output_motor_power(0,
                          50-offset+compass_offset,
                          50-offset-compass_offset,
                          0);
          break;
        case 9:
        case 10:
          // go_back
          set_motor_value(52-offset-compass_offset,
                          50-offset+compass_offset,
                          53-offset-compass_offset,
                          50-offset+compass_offset);
                          Serial.print("back ");
          output_motor_power(52-offset-compass_offset,
                          50-offset+compass_offset,
                          53-offset-compass_offset,
                          50-offset+compass_offset);
          break;
        default:
          // !?
          Serial.print("stop");
          set_motor_value(0, 0, 0, 0);
      }
    }
  }
  if(true) { // debug
    founded_ball = 1;
  } else if(nearest_ir[0] == -1) {
    // far ir
    Serial.println(0);
  } else {
    int ir_sub = abs(near_ir_value[nearest_ir[0]]-near_ir_value[nearest_ir[1]]);
    if(ir_sub > 30) {
      switch(nearest_ir[0]) {
      case 0: set_motor_value(go_front); break;
      case 1: set_motor_value(go_left); break;
      case 2: set_motor_value(go_right); break;
      case 3: set_motor_value(go_left_back); break;
      case 4: set_motor_value(go_right_back); break;
      case 5: set_motor_value(go_back); break;
      case 6: set_motor_value(go_back); break;
      case 7: set_motor_value(go_left); break;
      default: set_motor_value(motor_stop);
      }
      founded_ball = 1;
    } else {
      if((nearest_ir[0] == 0 && nearest_ir[1] == 1) || (nearest_ir[0] == 1 && nearest_ir[1] == 0)) {
        set_motor_value(go_left_front);
      } else if((nearest_ir[0] == 1 && nearest_ir[1] == 3) || (nearest_ir[0] == 3 && nearest_ir[1] == 1)) {
        set_motor_value(go_left);
      } else if((nearest_ir[0] == 3 && nearest_ir[1] == 5) || (nearest_ir[0] == 5 && nearest_ir[1] == 3)) {
        set_motor_value(go_left_back);
      } else if((nearest_ir[0] == 5 && nearest_ir[1] == 7) || (nearest_ir[0] == 7 && nearest_ir[1] == 5)) {
        set_motor_value(go_back);
      } else if((nearest_ir[0] == 7 && nearest_ir[1] == 6) || (nearest_ir[0] == 6 && nearest_ir[1] == 7)) {
        set_motor_value(go_back);
      } else if((nearest_ir[0] == 6 && nearest_ir[1] == 4) || (nearest_ir[0] == 4 && nearest_ir[1] == 6)) {
        set_motor_value(go_right_back);
      } else if((nearest_ir[0] == 4 && nearest_ir[1] == 2) || (nearest_ir[0] == 2 && nearest_ir[1] == 4)) {
        set_motor_value(go_right);
      } else if((nearest_ir[0] == 2 && nearest_ir[1] == 0) || (nearest_ir[0] == 2 && nearest_ir[1] == 0)) {
        set_motor_value(go_right_front);
      } else {
        set_motor_value(motor_stop);
      }
      founded_ball = 1;
    }
  }
  delay(10);
  move();
  delay(10);
}

// =======================================================================
// compass sensor
// return: 0~3599
// =======================================================================
int compass_read() {
  Wire.beginTransmission(COMPASS_ADDRESS);
  Wire.write("A");
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(COMPASS_ADDRESS, 2);
  byte heading_data[2] = {};
  int i=0;
  while(Wire.available() && i < 2) {
    heading_data[i] = Wire.read();
    ++i;
  }
  return heading_data[0]*256 + heading_data[1];
}

// =======================================================================
// 
// =======================================================================
int compass_from_reference() {
  compass_value = compass_read();
  int tmp_ref = compass_reference - compass_value;
  int compass_width = 150;
  if(tmp_ref < 0) {
    tmp_ref += 3600;
  }
  //Serial.print(compass_reference); Serial.print(" ");
  //Serial.print(compass_value); Serial.print(" ");
  //Serial.println(tmp_ref);
  if(tmp_ref < 1800) {
    return tmp_ref;
  } else {
    return tmp_ref-3600;
  }
  return 0;
}

// =======================================================================
// adjust direction
// return: if turn, return 1. otherwise 0.
// =======================================================================
int adjust_direction(int from_reference) {
  const int range = 150;
  const int offset = from_reference/90;
  if(from_reference > range) {
    set_motor_value(-20-offset, 20+offset, -20-offset, 20+offset);
  } else if(from_reference < -range) {
    set_motor_value(20-offset, -20+offset, 20-offset, -20+offset);
  } else {
    return 0;
  }
  return 1;
}

// =======================================================================
// read near ir sensor
// note: return first and second largest pin
// param: array of int, the size is 2
// =======================================================================
void read_ping() {
  Wire.requestFrom(PING_ADDRESS, PING_PIN_NUM);
  int i=0;
  while(Wire.available() && i < PING_PIN_NUM) {
    ping_value[i] = Wire.read();
    //Serial.println(ping_value[i]);
    ++i;
  }
  Wire.endTransmission();
}

// =======================================================================
// read near ir sensor
// note: return first and second largest pin
// param: array of int, the size is 2
// =======================================================================
void read_near_ir(int* ret) {
  int first=-1;
  for(int i=0; i<NEAR_IR_NUM; ++i) {
    near_ir_value[i] = analogRead(near_ir_pin[i]) * 100 / 1023;
    //Serial.print(near_ir_value[i]); Serial.print(" ");
    if(first == -1 || near_ir_value[i] < near_ir_value[first]) {
      first = i;
    }
  }
  Serial.println();
  if(near_ir_value[first] > 50) {
    ret[0] = -1;
    return;
  }

  int next1=-1, next2=-1;
  if(first == 0) {
    next1 = 1; next2 = 2;
  } else if(first == 1) {
    next1 = 0; next2 = 3;
  } else if(first == 7) {
    next1 = 5; next2 = 6;
  } else {
    next1 = first-2; next2 = first+2;
  }
  ret[0] = first;
  if(near_ir_value[next1] < near_ir_value[next2]) {
    ret[1] = next2;
  } else {
    ret[1] = next1;
  }
}

// =======================================================================
// read far ir sensor
// note: return first and second largest pin
// param: array of int, the size is 2
// =======================================================================
int read_far_ir() {
  Wire.requestFrom(FAR_IR_ADDRESS, 1);
  int data=0;
  if(Wire.available()) {
    data = Wire.read();
  }
  //Serial.println(data);
  far_ir_num = data/10;
  far_ir_status = data%10;
}

// =======================================================================
// motor control
// Daisen 6ch(4ch) motor control board
// input:-100~100 stop:0 brake:255
// =======================================================================
void set_motor_value(int vals[MOTOR_NUM]) {
  for(int i=0; i<MOTOR_NUM; ++i) {
    if(vals[i] < 0 && vals[i] >= -100) {
      if(motor_status[i] == 2) {
        motor_pow[i] = 0;
        motor_status[i] = 0;
      } else {
        vals[i] = abs(vals[i]);
        motor_pow[i] = vals[i] + 128;
        motor_status[i] = 1;
      }
    } else if(vals[i] > 0 && vals[i] <= 100) {
      if(motor_status[i] == 1) {
        motor_pow[i] = 0;
        motor_status[i] = 0;
      } else {
        motor_pow[i] = vals[i];
        motor_status[i] = 2;
      }
    } else if(vals[i] == 0 || vals[i] == 255) {
      motor_pow[i] = 128;
      motor_status[i] = 0;
    } else {
      return;
    }
  }
}
void set_motor_value(char val1, char val2, char val3, char val4) {
  int vals[MOTOR_NUM] = {val1, val2, val3, val4};
  set_motor_value(vals);
}

// =======================================================================
// motor move
// =======================================================================
void move(){
  Wire.beginTransmission(MOTOR_ADDRESS);
  for(int i=0; i<MOTOR_NUM; ++i) {
    Wire.write(motor_pow[i]);
  }
  Wire.write(0); // dummy
  Wire.write(0); // dummy
  Wire.endTransmission();
}

void output_motor_power(int a, int b, int c, int d) {
  Serial.print(a); Serial.print(" ");
  Serial.print(b); Serial.print(" ");
  Serial.print(c); Serial.print(" ");
  Serial.print(d); Serial.println();
}
