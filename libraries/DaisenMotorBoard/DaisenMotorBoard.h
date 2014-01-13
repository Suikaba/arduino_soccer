#ifndef DAISEN_MOTOR_BOARD_h
#define DAISEN_MOTOR_BOARD_h

#include <inttypes.h>
#include <Wire.h>


class daisen_motor_board
{
public:
    daisen_motor_board();

public:
    void set_motor_value(char p1, char p2, char p3, char p4);
    void set_motor_value(char pows[4]);
    void move();

private:
    int tmp_motor_power_[4];
    int motor_power_[4];
    int motor_status_[4];
};

#endif
