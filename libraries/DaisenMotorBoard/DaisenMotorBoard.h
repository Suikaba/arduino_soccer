#ifndef DAISEN_MOTOR_BOARD_h
#define DAISEN_MOTOR_BOARD_h

#include <inttypes.h>
#include <Wire.h>


class daisen_motor_board
{
public:
    daisen_motor_board();

public:
    void set_motor_value(int p1, int p2, int p3, int p4);
    void set_motor_value(int pows[4]);
    void move();

private:
    int tmp_motor_power_[4];
    int motor_power_[4];
    int motor_status_[4];
};

#endif
