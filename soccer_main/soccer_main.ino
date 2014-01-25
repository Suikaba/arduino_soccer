#include <Wire.h>
#include <DaisenMotorBoard.h>
#include <HMC5883L.h>
#include <LiquidCrystal.h>


const int MOTOR_NUM    = 4;
const int PING_PIN_NUM = 4;
const int NEAR_IR_NUM  = 8;


const int SWITCH1 = 48;
const int SWITCH2 = 50;
const int LED1    = 49;
const int LED2    = 51;
const int LED3    = 53;
const int NEAR_IR_PIN[] = {0, 1, 2, 8, 4, 5, 6, 7};
const int LIGHT_PIN[3][4] = {{43, 41, 39, 37}, {36, 38, 40, 42}, {44, 46, 47, 45}};


const int FAR_IR_ADDRESS = 0x10;
const int PING_ADDRESS   = 0x16;


const int FRONT_DIRECTION       = 0;
const int BACK_DIRECTION        = 1;
const int LEFT_DIRECTION        = 2;
const int RIGHT_DIRECTION       = 3;
const int LEFT_FRONT_DIRECTION  = 4;
const int RIGHT_FRONT_DIRECTION = 5;
const int LEFT_BACK_DIRECTION   = 6;
const int RIGHT_BACK_DIRECTION  = 7;

const int COMPASS_RANGE = 150;


void calc_compass_reference();
int read_compass();
int compass_from_reference();
void adjust_direction(int from_reference);
void read_ping();
void read_near_ir();
void read_far_ir();
void read_light_sensor();
int get_direction_from_near_ir();
int get_direction_from_far_ir();


DaisenMotorBoard motor_board;
HMC5883L compass;
LiquidCrystal lcd(23, 35, 25, 33, 27, 31, 29);
int ping_value[PING_PIN_NUM] = {0};
unsigned long near_ir_value[NEAR_IR_NUM] = {0};
int first_nearest_ir = -1;
int compass_reference = 0;
int largest_far_ir = -1;
int far_ir_status = -1;
bool is_on_line[3] = {false};


void setup(){
    Wire.begin();
    compass = HMC5883L();
    Serial.begin(9600);

    int error = compass.set_scale(1.3);
    if(error != 0) {
        // print(compass.get_error_text(error))
    }
    delay(20);
    if(!compass.calibrate(32)) {
        // failed calibration
    }
    error = compass.set_measurement_mode(MEASUREMENT_CONTINUOUS);
    if(error != 0) {
        // print ...
    }
    delay(50);
    read_compass(); // ignore first value
    calc_compass_reference();
}

void loop(){
    //unsigned int start = millis();
    int motor_stop[] = {0, 0, 0, 0};
    int go_front[] = {0, 0, 0, 0};
    int go_back[] = {0, 0, 0, 0};
    int go_left[] = {0, 0, 0, 0};
    int go_right[] = {0, 0, 0, 0};
    int go_left_front[] = {0, 0, 0, 0};
    int go_right_front[] = {0, 0, 0, 0};
    int go_left_back[] = {0, 0, 0, 0};
    int go_right_back[] = {0, 0, 0, 0};
    if(false) { // mat
        go_front[0]=-80; go_front[1]=-81; go_front[2]=-80; go_front[3]=-81;
        go_back[0]=80; go_back[1]=81; go_back[2]=80; go_back[3]=81;
        go_left[0]=80; go_left[1]=-80; go_left[2]=-83; go_left[3]=83;
        go_right[0]=-80; go_right[1]=80; go_right[2]=83; go_right[3]=-83;
        go_left_front[0]=0; go_left_front[1]=-75; go_left_front[2]=-75; go_left_front[3]=0;
        go_right_front[0]=-74; go_right_front[1]=0; go_right_front[2]=0; go_right_front[3]=-70;
        go_left_back[0]=74; go_left_back[1]=0; go_left_back[2]=0; go_left_back[3]=70;
        go_right_back[0]=0; go_right_back[1]=75; go_right_back[2]=75; go_right_back[3]=0;
    } else {
        go_front[0]=-50; go_front[1]=-51; go_front[2]=-50; go_front[3]=-51;
        go_back[0]=50; go_back[1]=51; go_back[2]=50; go_back[3]=51;
        go_left[0]=50; go_left[1]=-49; go_left[2]=-50; go_left[3]=50;
        go_right[0]=-51; go_right[1]=50; go_right[2]=50; go_right[3]=-50;
        go_left_front[0]=0; go_left_front[1]=-50; go_left_front[2]=-50; go_left_front[3]=0;
        go_right_front[0]=-55; go_right_front[1]=0; go_right_front[2]=0; go_right_front[3]=-52;
        go_left_back[0]=50; go_left_back[1]=0; go_left_back[2]=0; go_left_back[3]=55;
        go_right_back[0]=0; go_right_back[1]=50; go_right_back[2]=55; go_right_back[3]=0;
    }

    const int from_reference = compass_from_reference();
    read_far_ir();
    read_light_sensor();
            
    if(false/*is_on_line[0] || is_on_line[1] || is_on_line[2]*/) {
        // light process
    } else if(abs(from_reference) >= 750) {
        adjust_direction(from_reference);
        lcd.clear();
    } else if(far_ir_status == 0) {
        read_ping();
        if(abs(from_reference) >= COMPASS_RANGE) {
            adjust_direction(from_reference);
            lcd.clear();
        } else {
            // left - right
            const int side_sub = ping_value[2] - ping_value[3];
            if(ping_value[1] > 40) {
                if(side_sub < -20) {
                    motor_board.set_motor_value(go_right_back);
                } else if(side_sub > 20) {
                    motor_board.set_motor_value(go_left_back);
                } else {
                    motor_board.set_motor_value(go_back);
                }
            } else if(ping_value[1] < 12) {
                if(side_sub < -20) {
                    motor_board.set_motor_value(go_left_front);
                } else if(side_sub > 20) {
                    motor_board.set_motor_value(go_right_front);
                } else {
                    motor_board.set_motor_value(0, 0, 0, 0);
                }
            } else {
                if(side_sub < -20) {
                    motor_board.set_motor_value(go_right);
                } else if(side_sub > 20) {
                    motor_board.set_motor_value(go_left);
                } else {
                    motor_board.set_motor_value(0, 0, 0, 0);
                }
            }
        }
    } else if(far_ir_status >= 5) {
        delay(5);
        read_near_ir();
        const int compass_offset = from_reference/45;
        const int dir = get_direction_from_near_ir();
        switch(dir) {
        case FRONT_DIRECTION:
            motor_board.set_motor_value(
                  go_front[0]+compass_offset,
                  go_front[1]-compass_offset,
                  go_front[2]+compass_offset,
                  go_front[3]-compass_offset);
            lcd.clear();
            lcd.print("front n");
            break;
        case BACK_DIRECTION:
            // ignore offset
            motor_board.set_motor_value(
                go_back[0],
                go_back[1],
                go_back[2],
                go_back[3]);
            lcd.clear();
            lcd.print("back n");
            break;
        case LEFT_DIRECTION:
            motor_board.set_motor_value(
                go_left[0]+compass_offset,
                go_left[1]-compass_offset,
                go_left[2]+compass_offset,
                go_left[3]-compass_offset);
            lcd.clear();
            lcd.print("left n");
            break;
        case RIGHT_DIRECTION:
            motor_board.set_motor_value(
                go_right[0]+compass_offset,
                go_right[1]-compass_offset,
                go_right[2]+compass_offset,
                go_right[3]-compass_offset);
            lcd.clear();
            lcd.print("right n");
            break;
        case LEFT_FRONT_DIRECTION:
            motor_board.set_motor_value(
                0,
                go_left_front[1]-compass_offset,
                go_left_front[2]+compass_offset,
                0);
            lcd.clear();
            lcd.print("left_front n");
            break;
        case RIGHT_FRONT_DIRECTION:
            motor_board.set_motor_value(
                go_right_front[0]+compass_offset,
                0,
                0,
                go_right_front[4]-compass_offset);
            lcd.clear();
            lcd.print("right_front n");
            break;
        case LEFT_BACK_DIRECTION:
            motor_board.set_motor_value(
                go_left_back[0]-compass_offset,
                0,
                0,
                go_left_back[4]+compass_offset);
            lcd.clear();
            lcd.print("left_back n");
            break;
        case RIGHT_BACK_DIRECTION:
            motor_board.set_motor_value(
                0,
                go_right_back[1]-(compass_offset/2),
                go_right_back[2]+(compass_offset/2),
                0);
            lcd.clear();
            lcd.print("right_back n");
            break;
        default:
          // !?
          motor_board.set_motor_value(0, 0, 0, 0);
          lcd.clear();
        }
    } else {
        const int compass_offset = from_reference/45;
        const int dir = get_direction_from_far_ir();
        switch(dir) {
        case FRONT_DIRECTION:
            motor_board.set_motor_value(
                go_front[0]+compass_offset,
                go_front[1]-compass_offset,
                go_front[2]+compass_offset,
                go_front[3]-compass_offset);
            lcd.clear();
            lcd.print("front far");
            break;
        case BACK_DIRECTION:
            motor_board.set_motor_value(
                // ignore offset
                go_back[0],
                go_back[1],
                go_back[2],
                go_back[3]);
            lcd.clear();
            lcd.print("back far");
            break;
        case LEFT_DIRECTION:
            motor_board.set_motor_value(
                go_left[0]+compass_offset,
                go_left[1]-compass_offset,
                go_left[2]+compass_offset,
                go_left[3]-compass_offset);
            lcd.clear();
            Serial.println("left far");
            break;
        case RIGHT_DIRECTION:
            motor_board.set_motor_value(
                go_right[0]+compass_offset,
                go_right[1]-compass_offset,
                go_right[2]+compass_offset,
                go_right[3]-compass_offset);
            lcd.clear();
            lcd.print("right far");
            break;
        case LEFT_FRONT_DIRECTION:
            motor_board.set_motor_value(
                0,
                go_left_front[1]-compass_offset,
                go_left_front[2]+compass_offset,
                0);
            lcd.clear();
            lcd.print("left_front far");
            break;
        case RIGHT_FRONT_DIRECTION:
            motor_board.set_motor_value(
                go_right_front[0]+compass_offset,
                0,
                0,
                go_right_front[4]-compass_offset);
            lcd.clear();
            lcd.print("right_front far");
            break;
        case LEFT_BACK_DIRECTION:
            motor_board.set_motor_value(
                go_left_back[0]+compass_offset,
                0,
                0,
                go_left_back[4]-compass_offset);
            lcd.clear();    
            lcd.print("left_back far");
            break;
        case RIGHT_BACK_DIRECTION:
            motor_board.set_motor_value(
                0,
                go_right_back[1]-compass_offset,
                go_right_back[2]+compass_offset,
                0);
            lcd.clear();
            lcd.print("right_back far");
            break;
        default:
          motor_board.set_motor_value(0, 0, 0, 0);
          lcd.clear();
        }
    }
    motor_board.move();
    delay(20);
    //unsigned int end = millis();
}

void calc_compass_reference() {
    int compass_sum = 0;
    for(int i=0; i<2; ++i) {
        compass_sum += read_compass();
        delay(10);
    }
    compass_reference = compass_sum/2;
}

// @return 0~3599
int read_compass() {
    return (int)compass.read_heading() * 10;
}

// @return -1799~1799
int compass_from_reference() {
    const int compass_value = read_compass();
    const int tmp = compass_reference - compass_value;
    if(tmp < -1800) {
        return tmp + 3600;
    } else if(tmp > 1800) {
        return tmp - 3600;
    } else {
        return tmp;
    }
}

void adjust_direction(int from_reference) {
    const int offset = from_reference/50;
    if(from_reference > COMPASS_RANGE) {
        motor_board.set_motor_value(-14-offset, 14+offset, -14-offset, 14+offset);
    } else if(from_reference < -COMPASS_RANGE) {
        motor_board.set_motor_value(14-offset, -14+offset, 14-offset, -14+offset);
    } else {
        motor_board.set_motor_value(0, 0, 0, 0);
    }
}

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

void read_near_ir() {
    first_nearest_ir = -1;
    for(int i=0; i<NEAR_IR_NUM; ++i) {
        near_ir_value[i] = analogRead(NEAR_IR_PIN[i]) * 100 / 1023;
        //Serial.print(near_ir_value[i]); Serial.print(" ");
        if(i != 3 && (first_nearest_ir == -1 || near_ir_value[i] < near_ir_value[first_nearest_ir])) {
          first_nearest_ir = i;
        }
    }
    if(10 < near_ir_value[1] && near_ir_value[1] < 30 && 10 < near_ir_value[5] && near_ir_value[5] < 30) {
        first_nearest_ir=3;
    }
}

void read_far_ir() {
    Wire.requestFrom(FAR_IR_ADDRESS, 2);
    int i=0;
    if(Wire.available()) {
        int data = Wire.read();
        //Serial.println(data);
        largest_far_ir = data/10;
        far_ir_status = data%10;
        ++i;
        //Serial.print(largest_far_ir[i]); Serial.print(" ");
    }
    //Serial.println();
}

void read_light_sensor()
{
    for(int i=0; i<3; ++i) {
        is_on_line[i] = false;
        for(int j=0; j<4; ++j) {
            if(!is_on_line[i] && digitalRead(LIGHT_PIN[i][j]) == 0) {
                is_on_line[i] = true;
            }
        }
    }
}

int get_direction_from_near_ir()
{
    if(first_nearest_ir == 0) {
        if((near_ir_value[1] >= 20 && near_ir_value[2] >= 20) || abs(near_ir_value[1]-near_ir_value[2]) < 6) {
            return FRONT_DIRECTION;
        } else if(near_ir_value[1] < near_ir_value[2]) {
            return LEFT_FRONT_DIRECTION;
        } else {
            return RIGHT_FRONT_DIRECTION;
        }
    } else if(first_nearest_ir == 1) {
        if(near_ir_value[0] >= 20 && near_ir_value[3] >= 20) {
            return LEFT_DIRECTION;
        } else if(near_ir_value[0] < near_ir_value[3]) {
            return LEFT_FRONT_DIRECTION;
        } else {
            return LEFT_DIRECTION;
        }
    } else if(first_nearest_ir == 2) {
        return RIGHT_DIRECTION;
    } else if(first_nearest_ir == 3) {
        return LEFT_BACK_DIRECTION;
    } else if(first_nearest_ir == 4) {
        return RIGHT_BACK_DIRECTION;
    } else if(first_nearest_ir == 5) {
        return BACK_DIRECTION;
    } else if(first_nearest_ir == 6) {
        return BACK_DIRECTION;
    } else if(first_nearest_ir == 7) {
        return BACK_DIRECTION;
    }
    return -1;
    lcd.print("hoge");
}

int get_direction_from_far_ir()
{
    if(largest_far_ir == 0) {
        return FRONT_DIRECTION;
    } else if(largest_far_ir == 1) {
        return LEFT_FRONT_DIRECTION;
    } else if(largest_far_ir == 2) {
        return RIGHT_FRONT_DIRECTION;
    } else if(largest_far_ir == 3) {
        return LEFT_DIRECTION;
    } else if(largest_far_ir == 4) {
        return RIGHT_DIRECTION;
    } else if(largest_far_ir == 5) {
        return LEFT_BACK_DIRECTION;
    } else if(largest_far_ir == 6) {
        return RIGHT_BACK_DIRECTION;
    } else if(largest_far_ir == 7) {
        return BACK_DIRECTION;
    } else if(largest_far_ir == 8) {
        return BACK_DIRECTION;
    }
    return -1;
}

void output_motor_power(int a, int b, int c, int d) {
    Serial.print("motor_power:");
    Serial.print(a); Serial.print(" ");
    Serial.print(b); Serial.print(" ");
    Serial.print(c); Serial.print(" ");
    Serial.print(d); Serial.println();
}
