/*
HMC5883L.h - Header file for the HMC5883L Triple Axis Magnetometer Arduino Library.
Copyright (C) 2011 Love Electronics (loveelectronics.co.uk)

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

 WARNING: THE HMC5883L IS NOT IDENTICAL TO THE HMC5883!
 Datasheet for HMC5883L:
 http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/HMC5883L_3-Axis_Digital_Compass_IC.pdf

*/

#ifndef HMC5883L_h
#define HMC5883L_h

#include <inttypes.h>
#include <Wire.h>

#define HMC5883L_ADDRESS          0x1E
#define HMC_POS_BIAS              1
#define HMC_NEG_BIAS              2
#define CONFIGURATION_REGISTER_A  0x00
#define CONFIGURATION_REGISTER_B  0x01
#define MODE_REGISTER             0x02
#define DATA_REGISTER_BEGIN       0x03
#define STATUS_REGISTER           0x09
#define IDENTIFICATION_REGISTER_A 0x0A
#define IDENTIFICATION_REGISTER_B 0x0B
#define IDENTIFICATION_REGISTER_C 0x0C

#define MEASUREMENT_CONTINUOUS 0x00
#define MEASUREMENT_SINGLESHOT 0x01
#define MEASUREMENT_IDLE       0x03

#define X_SELF_TEST_GAUSS    (+1.16)
#define Y_SELF_TEST_GAUSS    (X_SELF_TEST_GAUSS)
#define Z_SELF_TEST_GAUSS    (+1.08)
#define SELF_TEST_LOW_LIMIT  (243.0/390.0)
#define SELF_TEST_HIGH_LIMIT (575.0/390.0)

#define ERROR_CODE_1 "Entered scale was not valid, valid gauss values are: 0.88, 1.3, 1.9, 2.5, 4.0, 4.7, 5.6, 8.1"
#define ERROR_CODE_1_NUM 1

struct magnetometer_scaled
{
    float x_axis;
    float y_axis;
    float z_axis;
};

struct magnetometer_raw
{
    int x_axis;
    int y_axis;
    int z_axis;
};

class HMC5883L
{
public:
    HMC5883L();

    magnetometer_raw read_raw_axis();
    magnetometer_scaled read_scaled_axis();

    int set_measurement_mode(uint8_t mode);
    int set_scale(float gauss);
    void get_id(char i[3]);

    void calibrate(int seconds);
    //bool calibrate(int samples_num);

    float read_heading(); // degree, not radian

    char* get_error_text(int error_code);

private:
    void write(int address, int byte);
    uint8_t* read(int address, int length);

private:
    float x_scale_, y_scale_, z_scale_;
    float x_offset_, y_offset_;
};

#endif
