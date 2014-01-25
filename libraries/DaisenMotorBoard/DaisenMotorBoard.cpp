
#include <WProgram.h>
#include <DaisenMotorBoard.h>


#define MOTOR_ADDRESS (0x14>>1)
#define MOTOR_NUM 4

#define MOTOR_STATUS_STOP  0
#define MOTOR_STATUS_FROMT 1
#define MOTOR_STATUS_BACK  2


DaisenMotorBoard::DaisenMotorBoard() {
    for(int i=0; i<MOTOR_NUM; ++i) {
        tmp_motor_power_[i] = 0;
        motor_power_[i] = 0;
        motor_status_[i]= MOTOR_STATUS_STOP;
    }
}

void DaisenMotorBoard::set_motor_value(int p1, int p2, int p3, int p4) {
    int pows[4] = {p1, p2, p3, p4};
    set_motor_value(pows);
}

void DaisenMotorBoard::set_motor_value(int pows[4]) {
    for(int i=0; i<MOTOR_NUM; ++i) {
        if(pows[i] < 0 && pows[i] >= -100) {
            if(motor_status_[i] == MOTOR_STATUS_BACK) {
              tmp_motor_power_[i] = 0;
              motor_status_[i] = MOTOR_STATUS_STOP;
            } else {
              pows[i] = abs(pows[i]);
              tmp_motor_power_[i] = pows[i] + 128;
              motor_status_[i] = MOTOR_STATUS_FROMT;
            }
        } else if(pows[i] > 0 && pows[i] <= 100) {
            if(motor_status_[i] == MOTOR_STATUS_FROMT) {
              tmp_motor_power_[i] = 0;
              motor_status_[i] = MOTOR_STATUS_STOP;
            } else {
              tmp_motor_power_[i] = pows[i];
              motor_status_[i] = MOTOR_STATUS_BACK;
            }
        } else if(pows[i] == 0 || pows[i] == 255) {
            tmp_motor_power_[i] = 128;
            motor_status_[i] = MOTOR_STATUS_STOP;
        } else {
            return;
        }
    }
}

void DaisenMotorBoard::move()
{
    for(int i=0; i<MOTOR_NUM; ++i) {
        motor_power_[i] = tmp_motor_power_[i];
    }
    Wire.beginTransmission(MOTOR_ADDRESS);
    for(int i=0; i<MOTOR_NUM; ++i) {
        Wire.write(motor_power_[i]);
    }
    Wire.write(0); // dummy
    Wire.write(0);
    Wire.endTransmission();
}
