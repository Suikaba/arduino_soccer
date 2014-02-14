/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Magnetometer Arduino Library*/

#include <Arduino.h>
#include <HMC5883L.h>

namespace {
    // const int counts_per_milligauss[8] =
    // {
    //     1370, 1090, 820, 660, 440, 390, 330, 230
    // };
    const int milligauss = 1090; // gain 0x01
}

HMC5883L::HMC5883L()
{
    x_scale_ = 1.f;
    y_scale_ = 1.f;
    z_scale_ = 1.f;
}

magnetometer_raw HMC5883L::read_raw_axis()
{
  uint8_t* buffer = read(DATA_REGISTER_BEGIN, 6);
  magnetometer_raw raw = {};
  raw.x_axis = (short)((buffer[0] << 8) | buffer[1]);
  raw.z_axis = (short)((buffer[2] << 8) | buffer[3]);
  raw.y_axis = (short)((buffer[4] << 8) | buffer[5]);
  //if(raw.x_axis > 32767) raw.x_axis -= 65535;
  //if(raw.y_axis > 32767) raw.y_axis -= 65535;
  //if(raw.z_axis > 32767) raw.z_axis -= 65535;
  return raw;
}

magnetometer_scaled HMC5883L::read_scaled_axis()
{
  magnetometer_raw raw = read_raw_axis();
  magnetometer_scaled scaled = {};
  scaled.x_axis = raw.x_axis * x_scale_;
  scaled.z_axis = raw.z_axis * z_scale_;
  scaled.y_axis = raw.y_axis * y_scale_;
  return scaled;
}

int HMC5883L::set_scale(float gauss)
{
  uint8_t reg_value = 0x00;
  if(gauss == 0.88f)
  {
    reg_value = 0x00;
    x_scale_ = 0.73;
    y_scale_ = 0.73;
    z_scale_ = 0.73;
  }
  else if(gauss == 1.3f)
  {
    reg_value = 0x01;
    x_scale_ = 0.92;
    y_scale_ = 0.92;
    z_scale_ = 0.92;
  }
  else if(gauss == 1.9f)
  {
    reg_value = 0x02;
    x_scale_ = 1.22;
    y_scale_ = 1.22;
    z_scale_ = 1.22;
  }
  else if(gauss == 2.5f)
  {
    reg_value = 0x03;
    x_scale_ = 1.52;
    y_scale_ = 1.52;
    z_scale_ = 1.52;
  }
  else if(gauss == 4.0f)
  {
    reg_value = 0x04;
    x_scale_ = 2.27;
    y_scale_ = 2.27;
    z_scale_ = 2.27;
  }
  else if(gauss == 4.7f)
  {
    reg_value = 0x05;
    x_scale_ = 2.56;
    y_scale_ = 2.56;
    z_scale_ = 2.56;
  }
  else if(gauss == 5.6f)
  {
    reg_value = 0x06;
    x_scale_ = 3.03;
    y_scale_ = 3.03;
    z_scale_ = 3.03;
  }
  else if(gauss == 8.1f)
  {
    reg_value = 0x07;
    x_scale_ = 4.35;
    y_scale_ = 4.35;
    z_scale_ = 4.35;
  }
  else
    return ERROR_CODE_1_NUM;

  // Setting is in the top 3 bits of the register.
  reg_value = reg_value << 5;
  write(CONFIGURATION_REGISTER_B, reg_value);
  return 0;
}

int HMC5883L::set_measurement_mode(uint8_t mode)
{
  write(MODE_REGISTER, mode);
  return 0;
}

void HMC5883L::calibrate()
{
    float mx=0.f, my=0.f, mz=0.f;
    write(CONFIGURATION_REGISTER_A, 0x010+HMC_POS_BIAS);
    for(int i=0; i<10; ++i)
    {
        magnetometer_scaled scaled = read_scaled_axis();
        mx = max(mx, scaled.x_axis);
        my = max(my, scaled.y_axis);
        mz = max(mz, scaled.z_axis);
    }
    float max_ = max(mx, max(my, mz));
    x_scale_ = mx/max_;
    y_scale_ = my/max_;
    z_scale_ = mz/max_;
    write(CONFIGURATION_REGISTER_A, 0x010); // set RegA/DOR back to default
}

bool HMC5883L::calibrate(int samples_num)
{
    long int xyz_total[3] = {0};
    char id[3] = {};
    bool bret = true;
    if(0 < samples_num)
    {
        get_id(id);
        if('H' == id[0] && '4' == id[1] && '3' == id[2])
        {
            write(CONFIGURATION_REGISTER_A, 0x010+HMC_POS_BIAS);
            set_measurement_mode(MEASUREMENT_SINGLESHOT);
            read_raw_axis(); // ignore first
            for(unsigned int i=0; i<samples_num; ++i)
            {
                set_measurement_mode(MEASUREMENT_SINGLESHOT);
                magnetometer_raw raw = read_raw_axis();
                xyz_total[0] += raw.x_axis;
                xyz_total[1] += raw.y_axis;
                xyz_total[2] += raw.z_axis;

                if(-(1<<12) >= min(raw.x_axis, min(raw.y_axis, raw.z_axis)))
                {
                    // todo
                    // print error message
                    bret = false;
                    break;
                }
            }
            write(CONFIGURATION_REGISTER_A, 0x010+HMC_NEG_BIAS);
            for(unsigned int i=0; i<samples_num; ++i)
            {
                set_measurement_mode(MEASUREMENT_SINGLESHOT);
                magnetometer_raw raw = read_raw_axis();
                xyz_total[0] -= raw.x_axis;
                xyz_total[1] -= raw.y_axis;
                xyz_total[2] -= raw.z_axis;

                if(-(1<<12) >= min(raw.x_axis, min(raw.y_axis, raw.z_axis)))
                {
                    // todo
                    // print error message
                    bret = false;
                    break;
                }
            }
            long int low_limit
                = SELF_TEST_LOW_LIMIT * milligauss * 2 * samples_num;
            long int high_limit
                = SELF_TEST_HIGH_LIMIT * milligauss * 2 * samples_num;
            if(bret &&
               low_limit <= xyz_total[0] && high_limit >= xyz_total[0] &&
               low_limit <= xyz_total[1] && high_limit >= xyz_total[1] &&
               low_limit <= xyz_total[2] && high_limit >= xyz_total[2])
            {
                // successful
                x_scale_
                    = (milligauss*(X_SELF_TEST_GAUSS*2))/(xyz_total[0]/samples_num);
                y_scale_
                    = (milligauss*(Y_SELF_TEST_GAUSS*2))/(xyz_total[1]/samples_num);
                z_scale_
                    = (milligauss*(Z_SELF_TEST_GAUSS*2))/(xyz_total[2]/samples_num);
            } else {
                bret = false;
            }
            write(CONFIGURATION_REGISTER_A, 0x010);
        } else {
            // failed id check
            bret = false;
        }
    } else {
        // bad input param
        bret = false;
    }
    return bret;
}

float HMC5883L::read_heading()
{
    magnetometer_scaled scaled = read_scaled_axis();
    float heading_rad = atan2(scaled.y_axis, scaled.x_axis);
    // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: 2� 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  //float declination_angle = 0.0457;
  //heading += declination_angle;
  if(heading_rad < 0) heading_rad += 2*PI;
  if(heading_rad > 2*PI) heading_rad -= 2*PI;

  return heading_rad * 180/PI;
}

void HMC5883L::write(int address, int data)
{
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t* HMC5883L::read(int address, int length)
{
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.requestFrom(HMC5883L_ADDRESS, length);

  uint8_t buffer[length];
  if(Wire.available() == length)
  {
    for(uint8_t i = 0; i < length; i++)
    {
      buffer[i] = Wire.read();
    }
  }
  Wire.endTransmission();

  return buffer;
}

void HMC5883L::get_id(char id[3])
{
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.write(IDENTIFICATION_REGISTER_A);
    Wire.endTransmission();
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.requestFrom(HMC5883L_ADDRESS, 3);
    if(3 == Wire.available())
    {
        for(int i=0; i<3; ++i) id[i] = Wire.read();
    } else {
        for(int i=0; i<3; ++i) id[i] = 0;
    }
    Wire.endTransmission();
}

char* HMC5883L::get_error_text(int error_code)
{
  if(ERROR_CODE_1_NUM == 1)
    return ERROR_CODE_1;
  else
    return "Error not defined.";
}
