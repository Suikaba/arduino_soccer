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
const int LIGHT_PIN[3][2] = {{9, 10}, {11, 12}, {13, 14}};


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

const int LINE_LEFT_FRONT  = 2;
const int LINE_RIGHT_FRONT = 0;
const int LINE_BACK        = 1;

const int LINE_FRONT_F       = 1;
const int LINE_BACK_F        = 2;
const int LINE_LEFT_F        = 3;
const int LINE_RIGHT_F       = 4;
const int LINE_LEFT_FRONT_F  = 5;
const int LINE_RIGHT_FRONT_F = 6;
const int LINE_LEFT_BACK_F   = 7;
const int LINE_RIGHT_BACK_F  = 8;


void calc_compass_reference();
int read_compass();
int compass_from_reference();
void read_switches();
void adjust_direction(int from_reference);
void read_ping();
void read_near_ir();
void read_far_ir();
void read_light_sensor();
void set_new_light_distance();
int get_direction_from_near_ir();
int get_direction_from_far_ir();
void output_motor_power(int, int, int, int);


DaisenMotorBoard motor_board;
HMC5883L compass;
LiquidCrystal lcd(23, 35, 25, 33, 27, 31, 29);
int ping_value[PING_PIN_NUM] = {0};
unsigned long near_ir_value[NEAR_IR_NUM] = {0};
int first_nearest_ir = -1;
int compass_reference = 0;
int light_reference[3][2] = {};
int largest_far_ir = -1;
int far_ir_status = -1;
bool switches[2] = {false};
bool is_on_line[3] = {false};
bool is_on_line2[3][2] = {false};
int light_flag = 0;
int before_light_flag = 0;
int before_before_light_flag = 0;
int light_distance[4] = {0};


void setup(){
    Wire.begin();
    pinMode(SWITCH1, INPUT);
    pinMode(SWITCH2, INPUT);
    compass = HMC5883L();
    //Serial.begin(9600);

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

    for(int i=0; i<3; ++i) {
        for(int j=0; j<2; ++j) {
            light_reference[i][j] = analogRead(LIGHT_PIN[i][j]);
        }
    }
}

void loop(){
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
        go_front[0]=-45; go_front[1]=-46; go_front[2]=-45; go_front[3]=-45;
        go_back[0]=45; go_back[1]=46; go_back[2]=46; go_back[3]=45;
        go_left[0]=45; go_left[1]=-44; go_left[2]=-45; go_left[3]=45;
        go_right[0]=-46; go_right[1]=45; go_right[2]=45; go_right[3]=-45;
        go_left_front[0]=0; go_left_front[1]=-45; go_left_front[2]=-45; go_left_front[3]=0;
        go_right_front[0]=-45; go_right_front[1]=0; go_right_front[2]=0; go_right_front[3]=-45;
        go_left_back[0]=45; go_left_back[1]=0; go_left_back[2]=0; go_left_back[3]=45;
        go_right_back[0]=0; go_right_back[1]=45; go_right_back[2]=46; go_right_back[3]=0;
    }

    lcd.clear();
    read_ping();
    read_switches();
    const int from_reference = compass_from_reference();
    read_far_ir();
    read_light_sensor();

    const int compass_offset = from_reference/37;
    go_front[0]-=compass_offset; go_front[1]+=compass_offset; go_front[2]-=compass_offset; go_front[3]+=compass_offset;
    go_back[0]-=compass_offset; go_back[1]+=compass_offset; go_back[2]-=compass_offset; go_back[3]+=compass_offset;
    go_left[0]-=compass_offset; go_left[1]+=compass_offset; go_left[2]-=compass_offset; go_left[3]+=compass_offset;
    go_right[0]-=compass_offset; go_right[1]+=compass_offset; go_right[2]-=compass_offset; go_right[3]+=compass_offset;
    go_left_front[1]+=compass_offset; go_left_front[2]-=compass_offset;
    go_right_front[0]-=compass_offset; go_right_front[3]+=compass_offset;
    go_left_back[0]+=compass_offset; go_left_back[3]-=compass_offset;
    go_right_back[1]+=compass_offset; go_right_back[2]+=compass_offset;


    if(!switches[0] || !switches[1]) {
        motor_board.set_motor_value(motor_stop);
        light_flag = 0;
        before_light_flag = 0;
    } else if(light_flag != 0 || is_on_line[LINE_LEFT_FRONT] || is_on_line[LINE_RIGHT_FRONT] || is_on_line[LINE_BACK]) {
        lcd.print("light");
        const int ping_range = 33+abs(from_reference/12);
        if(before_before_light_flag != 0 || (!is_on_line[LINE_LEFT_FRONT] && !is_on_line[LINE_RIGHT_FRONT] && !is_on_line[LINE_BACK])) {
            bool escaped = false;
            switch(light_flag) {
            case LINE_FRONT_F:
                if(ping_value[0] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_back);
                }
                break;
            case LINE_BACK_F:
                if(ping_value[1] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_front);
                }
                break;
            case LINE_LEFT_F:
                if(ping_value[2] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_right);
                }
                break;
            case LINE_RIGHT_F:
                if(ping_value[3] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_left);
                }
                break;
            case LINE_LEFT_FRONT_F:
                if(ping_value[2] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_right_back);
                }
                break;
            case LINE_RIGHT_FRONT_F:
                if(ping_value[3] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_left_back);
                }
                break;
            case LINE_LEFT_BACK_F:
                if(ping_value[2] > ping_range && ping_value[1] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_right_front);
                }
                break;
            case LINE_RIGHT_BACK_F:
                if(ping_value[3] > ping_range && ping_value[1] > ping_range) {
                    escaped = true;
                } else {
                    motor_board.set_motor_value(go_left_front);
                }
                break;
            }
            if(escaped) {
                before_before_light_flag = 0;
                before_light_flag = 0;
                light_flag = 0;
            }
        } else if(light_flag != 0) {
            lcd.clear();
            switch(light_flag) {
            case LINE_FRONT_F:
                if(is_on_line[LINE_BACK]) {
                    if(ping_value[0] < light_distance[0]) {
                        motor_board.set_motor_value(go_back);
                    } else if(from_reference < 0) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_LEFT_FRONT_F;
                        motor_board.set_motor_value(go_right_back);
                    } else {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_RIGHT_FRONT_F;
                        motor_board.set_motor_value(go_left_back);
                    }
                } else {
                    motor_board.set_motor_value(go_back);
                }
                lcd.print("LINE_FRONT_F");
                break;
            case LINE_BACK_F:
                if(before_before_light_flag == 0) {
                    if(is_on_line[LINE_LEFT_FRONT]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_LEFT_BACK_F;
                        light_distance[4] = ping_value[4];
                        motor_board.set_motor_value(go_right_front);
                    } else if(is_on_line[LINE_RIGHT_FRONT]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_RIGHT_BACK_F;
                        light_distance[3] = ping_value[3];
                        motor_board.set_motor_value(go_left_front);
                    } else {
                        motor_board.set_motor_value(go_front);
                    }
                } else {
                    motor_board.set_motor_value(go_front);
                }
                lcd.print("LINE_BACK_F");
                break;
            case LINE_LEFT_F:
                if(before_before_light_flag == 0) {
                    if(is_on_line[LINE_RIGHT_FRONT]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_FRONT_F;
                        light_distance[0] = ping_value[0];
                        motor_board.set_motor_value(go_back);
                    } else if(is_on_line[LINE_BACK]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_LEFT_BACK_F;
                        light_distance[1] = ping_value[1];
                        motor_board.set_motor_value(go_right_front);
                    } else {
                        motor_board.set_motor_value(go_right);
                    }
                } else {
                    motor_board.set_motor_value(go_right);
                }
                lcd.print("LINE_LEFT_F");
                break;
            case LINE_RIGHT_F:
                if(before_before_light_flag == 0) {
                    if(is_on_line[LINE_LEFT_FRONT]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_FRONT_F;
                        motor_board.set_motor_value(go_back);
                    } else if(is_on_line[LINE_BACK]) {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_RIGHT_BACK_F;
                        motor_board.set_motor_value(go_left_front);
                    } else {
                        motor_board.set_motor_value(go_left);
                    }
                } else {
                    motor_board.set_motor_value(go_left);
                }
                lcd.print("LINE_RIGHT_F");
                break;
            case LINE_LEFT_FRONT_F:
                motor_board.set_motor_value(go_right_back);
                lcd.print("LINE_LEFT_FRONT_F");
                break;
            case LINE_RIGHT_FRONT_F:
                motor_board.set_motor_value(go_left_back);
                lcd.print("LINE_RIGHT_FRONT_F");
                break;
            case LINE_LEFT_BACK_F:
                if(is_on_line[LINE_RIGHT_FRONT]) {
                    if(ping_value[1] < light_distance[1] || ping_value[2] < light_distance[2]) {
                        motor_board.set_motor_value(go_right_front);
                    } else {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_LEFT_FRONT_F;
                        motor_board.set_motor_value(go_right_back);
                    }
                } else {
                    motor_board.set_motor_value(go_right_front);
                }
                lcd.print("LINE_LEFT_BACK_F");
                break;
            case LINE_RIGHT_BACK_F:
                if(is_on_line[LINE_LEFT_FRONT]) {
                    if(ping_value[1] < light_distance[1] || ping_value[4] < light_distance[4]) {
                        motor_board.set_motor_value(go_left_front);
                    } else {
                        before_before_light_flag = before_light_flag;
                        before_light_flag = light_flag;
                        light_flag = LINE_RIGHT_FRONT_F;
                        motor_board.set_motor_value(go_left_back);
                    }
                } else {
                    motor_board.set_motor_value(go_left_front);
                }
                lcd.print("LINE_RIGHT_BACK_F");
                break;
            }
        } else if(is_on_line[LINE_LEFT_FRONT]) {
            if(is_on_line[LINE_RIGHT_FRONT]) {
                before_light_flag = -1;
                light_flag = LINE_FRONT_F;
                motor_board.set_motor_value(go_back);
            } else if(is_on_line[LINE_BACK]) {
                before_light_flag = -1;
                light_flag = LINE_LEFT_BACK_F;
                motor_board.set_motor_value(go_right_front);
            } else {
                light_flag = LINE_LEFT_F;
                motor_board.set_motor_value(go_right);
            }
            set_new_light_distance();
        } else if(is_on_line[LINE_RIGHT_FRONT]) {
            if(is_on_line[LINE_BACK]) {
                before_light_flag = -1;
                light_flag = LINE_RIGHT_BACK_F;
                motor_board.set_motor_value(go_left_front);
            } else {
                light_flag = LINE_RIGHT_F;
                motor_board.set_motor_value(go_left);
            }
            set_new_light_distance();
        } else if(is_on_line[LINE_BACK]) {
            light_flag = LINE_BACK_F;
            motor_board.set_motor_value(go_front);
            set_new_light_distance();
        }
    } else if(abs(from_reference) >= 900) {
        adjust_direction(from_reference);
    } else if(far_ir_status <= 2) {
        if(abs(from_reference) >= COMPASS_RANGE) {
            adjust_direction(from_reference);
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
        read_near_ir();
        const int dir = get_direction_from_near_ir();
        switch(dir) {
        case FRONT_DIRECTION:
            motor_board.set_motor_value(go_front);
            lcd.print("front n");
            break;
        case BACK_DIRECTION:
            motor_board.set_motor_value(go_back);
            lcd.print("back n");
            break;
        case LEFT_DIRECTION:
            motor_board.set_motor_value(go_left);
            lcd.print("left n");
            break;
        case RIGHT_DIRECTION:
            motor_board.set_motor_value(go_right);
            lcd.print("right n");
            break;
        case LEFT_FRONT_DIRECTION:
            motor_board.set_motor_value(go_left_front);
            lcd.print("left_front n");
            break;
        case RIGHT_FRONT_DIRECTION:
            motor_board.set_motor_value(go_right_front);
            lcd.print("right_front n");
            break;
        case LEFT_BACK_DIRECTION:
            motor_board.set_motor_value(go_left_back);
            lcd.print("left_back n");
            break;
        case RIGHT_BACK_DIRECTION:
            motor_board.set_motor_value(go_right_back);
            lcd.print("right_back n");
            break;
        default:
          motor_board.set_motor_value(0, 0, 0, 0);
        }
    } else {
        const int dir = get_direction_from_far_ir();
        switch(dir) {
        case FRONT_DIRECTION:
            motor_board.set_motor_value(go_front);
            lcd.print("front far");
            break;
        case BACK_DIRECTION:
            motor_board.set_motor_value(go_back);
            lcd.print("back far");
            break;
        case LEFT_DIRECTION:
            motor_board.set_motor_value(go_left);
            Serial.println("left far");
            break;
        case RIGHT_DIRECTION:
            motor_board.set_motor_value(go_right);
            lcd.print("right far");
            break;
        case LEFT_FRONT_DIRECTION:
            motor_board.set_motor_value(go_left_front);
            lcd.print("left_front far");
            break;
        case RIGHT_FRONT_DIRECTION:
            motor_board.set_motor_value(go_right_front);
            lcd.print("right_front far");
            break;
        case LEFT_BACK_DIRECTION:
            motor_board.set_motor_value(go_left_back);
            lcd.print("left_back far");
            break;
        case RIGHT_BACK_DIRECTION:
            motor_board.set_motor_value(go_right_back);
            lcd.print("right_back far");
            break;
        default:
          motor_board.set_motor_value(0, 0, 0, 0);
        }
    }
    motor_board.move();
    delay(20);
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

void read_switches() {
    switches[0] = digitalRead(SWITCH1);
    switches[1] = digitalRead(SWITCH2);
}

void adjust_direction(int from_reference) {
    const int offset = from_reference/60;
    if(from_reference > COMPASS_RANGE) {
        motor_board.set_motor_value(-10-offset, 10+offset, -10-offset, 10+offset);
    } else if(from_reference < -COMPASS_RANGE) {
        motor_board.set_motor_value(10-offset, -10+offset, 10-offset, -10+offset);
    } else {
        motor_board.set_motor_value(0, 0, 0, 0);
    }
}

void read_ping() {
    Wire.requestFrom(PING_ADDRESS, PING_PIN_NUM);
    int i=0;
    while(Wire.available() && i < PING_PIN_NUM) {
        ping_value[i] = Wire.read();
        ++i;
    }
    Wire.endTransmission();
}

void read_near_ir() {
    first_nearest_ir = -1;
    for(int i=0; i<NEAR_IR_NUM; ++i) {
        near_ir_value[i] = analogRead(NEAR_IR_PIN[i]) * 100 / 1023;
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
        largest_far_ir = data/10;
        far_ir_status = data%10;
        ++i;
    }
}

void read_light_sensor() {
    for(int i=0; i<3; ++i) {
        is_on_line[i] = false;
        for(int j=0; j<2; ++j) {
            int tmp = analogRead(LIGHT_PIN[i][j]);
            if(light_reference[i][j]-tmp > 90) {
                is_on_line[i] = true;
                is_on_line2[i][j] = true;
            }
        }
    }
}

void set_new_light_distance() {
    for(int i=0; i<4; ++i) {
        light_distance[i] = ping_value[i];
    }
}

int get_direction_from_near_ir() {
    if(first_nearest_ir == 0) {
        if((near_ir_value[1] >= 30 && near_ir_value[2] >= 30) || abs(near_ir_value[1]-near_ir_value[2]) < 10) {
            return FRONT_DIRECTION;
        } else if(near_ir_value[1] < near_ir_value[2]) {
            return LEFT_FRONT_DIRECTION;
        } else {
            return RIGHT_FRONT_DIRECTION;
        }
    } else if(first_nearest_ir == 1) {
        if(near_ir_value[0] <= 30 && near_ir_value[3] <= 30) {
            return LEFT_DIRECTION;
        } else if(near_ir_value[0] < near_ir_value[3]) {
            return LEFT_FRONT_DIRECTION;
        } else {
            return LEFT_DIRECTION;
        }
    } else if(first_nearest_ir == 2) {
        if(near_ir_value[0] <= 30 && near_ir_value[4] <= 30) {
            return RIGHT_DIRECTION;
        } else if(near_ir_value[0] < near_ir_value[4]) {
            return RIGHT_FRONT_DIRECTION;
        } else {
            return RIGHT_DIRECTION;
        }
    } else if(first_nearest_ir == 3) {
        return LEFT_BACK_DIRECTION;
    } else if(first_nearest_ir == 4) {
        return RIGHT_BACK_DIRECTION;
    } else if(first_nearest_ir == 5) {
        if(near_ir_value[first_nearest_ir]-near_ir_value[3]) {
            return LEFT_BACK_DIRECTION;
        } else {
            return BACK_DIRECTION;
        }
    } else if(first_nearest_ir == 6) {
        return BACK_DIRECTION;
    } else if(first_nearest_ir == 7) {
        const int sub_between_nexts = abs(near_ir_value[5]-near_ir_value[6]);
        if(near_ir_value[5] <= 30 && near_ir_value[6] <= 30 && sub_between_nexts < 13) {
            if(ping_value[2] < ping_value[3]) {
                return RIGHT_BACK_DIRECTION;
            } else {
                return LEFT_BACK_DIRECTION;
            }
        } else { // todo
            return BACK_DIRECTION;
        }
    }
}

int get_direction_from_far_ir() {
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
    } else { // 8
        return BACK_DIRECTION;
    }
}

// debug
void output_motor_power(int a, int b, int c, int d) {
    Serial.print("motor_power:");
    Serial.print(a); Serial.print(" ");
    Serial.print(b); Serial.print(" ");
    Serial.print(c); Serial.print(" ");
    Serial.print(d); Serial.println();
}
