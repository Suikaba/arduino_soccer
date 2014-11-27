//
// �R���p�X�Z���T�[ HMC5883L ��Example.
// ���C�u�����̓��������̏ڍׂ͒m��Ȃ��Ă��g����悤�ɂ��Ă܂�.
//
#include <Wire.h>
#include <HMC5883L.h>

// �R���p�X�{��
HMC5883L compass;
// �G���[�m�F�p�ϐ�
int error = 0;

void setup() {
    Serial.begin(9600); // Serial�̓e�X�g�ŏo�͂���Ȃ�K�v
    Wire.begin();       // ���̍s�͕K���K�v

    // �ȉ�println�̍s�͂��ׂăf�o�b�O�p�o��
    Serial.println("Constructing new HMC5883L");
    
    compass = HMC5883L();

    // �X�P�[���̐ݒ�. 1.3f�Őݒ肷��.
    Serial.println("Setting scale to +/- 1.3 Ga.");
    error = compass.set_scale(1.3f); // �G���[�łȂ����0���Ԃ��Ă���
    if(error != 0) {
        // �G���[�̏ڍׂ��o��
        Serial.println(compass.get_error_text(error));
    }

    // ���胂�[�h��ݒ肷��.
    // MESUREMENT_CONTINUOUS: �p�����胂�[�h. 
    //                        �֐����Ăяo���Ȃ��Ă����ŏ���Ƀf�[�^������Ă���Ă�.
    // MEASUREMENT_SINGLESHOT: �P������. �֐����Ă񂾃^�C�~���O�ő���J�n.
    // MEASUREMENT_IDLE: �悭�킩��Ȃ�. �ڍׂ̓`�[�g�V�[�g���Ă�������.
    // 
    // CONTINUOUS���[�h�ɂ��Ă����܂��傤.
    Serial.println("Setting measurement mode to continous.");
    error = compass.set_measurement_mode(MEASUREMENT_CONTINUOUS);
    if(error != 0) {
        Serial.println(compass.get_error_text(error));
    }

    delay(50); // �O�̂��߃Z���T�[�ނ̋N���ҋ@. �K�v�Ȃ�����


    // �L�����u���[�V����.
    // calibrate�́A�Ăяo���ƃL�����u���[�V�������I���܂ł����ŏ������~�܂�.
    // �L�����u���[�V�������ɁA�Z���T�[��360�x�������Ɖ�]�����邱��.
    // �ł��邾�������ȏꏊ�ōs��(�X�����肵�Ă͂Ȃ�Ȃ�).
    compass.calibrate(15); // 15�b�ԃL�����u���[�V�������s��.
}

void loop() {
    // ���f�[�^�̎擾. ���ʂ͎g�����Ƃ͂Ȃ��Ǝv���̂ł��܂�C�ɂ��Ȃ��Ă�����.
    magnetometer_raw raw = compass.read_raw_axis();

    //  ���f�[�^���X�P�[�������������Ƃ̃f�[�^�̎擾. ������g��Ȃ��Ǝv��.
    magnetometer_scaled scaled = compass.read_scaled_axis();

    // �������Ă�������̊p�x���擾. �ʓx�@(���W�A��)�ł͂Ȃ��x���@(0 ~ 359.9..).
    // �{�ԃv���O�����Ŏg�����ƂɂȂ�̂͂��̊֐����Ǝv��.
    float heading_degrees = compass.read_heading();
    // ���W�A���ɕϊ��������Ȃ�ȉ��̂悤�ɂ���.
    float heading_rad = heading_degrees * PI / 180;

    // �f�o�b�O�o��
    output(raw, scaled, heading_rad, heading_degrees);

    // �����̃f�B���C�̒l�͂͑��̃Z���T�[�Ƃ����k���Č��߂Ă�������.
    delay(30);
}


// �f�o�b�O�o�͗p.
void output(magnetometer_raw raw, magnetometer_scaled scaled, float heading, float heading_degrees)
{
   Serial.print("Raw: ");
   Serial.print(raw.x_axis);
   Serial.print(" ");   
   Serial.print(raw.y_axis);
   Serial.print(" ");   
   Serial.print(raw.z_axis);
   Serial.print("   Scaled: ");
   
   Serial.print(scaled.x_axis);
   Serial.print(" ");   
   Serial.print(scaled.y_axis);
   Serial.print(" ");   
   Serial.print(scaled.z_axis);

   Serial.print("   Heading: ");
   Serial.print(heading);
   Serial.print(" Radians. ");
   Serial.print(heading_degrees);
   Serial.println(" Degrees.");
}
