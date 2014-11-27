#include <Wire.h>

int compass_address = (0x42>>1);

void calibration() {
    // �R���p�X�Z���T�[�ƒʐM���J�n
    Wire.beginTransmission(compass_address);

    // �L�����u���[�V�����J�n�R�}���h���M. 
    // 'C'���R�}���h�ɂ�����(�`�[�g�V�[�g�ɏڍׂ���).
    // ��x�L�����u���[�V�����R�}���h�𑗐M����ƁA��~�R�}���h��
    // ���M����܂ŉi���ɃL�����u���[�V������������.
    // �������AHMC5883L�Ƃ͈���ăR�}���h���M���Ă��������Ƃ܂邱�Ƃ͂Ȃ�.
    // (�܂�A���ŏ���ɃL�����u���[�V�������Ă���Ă�Ƃ�������)
    // ��~�R�}���h�ɂ��Ă͌�q.
    Wire.write('C');

    // �R���p�X�Z���T�[�ƒʐM�ؒf
    Wire.endTransmission();

    // 15�b�ҋ@. �]�T�������Ă�������360�x��]��������悤�Ȏ��Ԃɂ���.
    delay(15000);

    Wire.beginTransmission(compass_address);

    // ��~�R�}���h���M
    Wire.write('E');

    Wire.endTransmission();
}


// 0 ~ 3600 �ŕԂ��Ă���(ex. 10�x�Ȃ�100)
int read_compass() {
    Wire.beginTransmission(compass_address);

    // �f�[�^�v���R�}���h���M.
    // �擾���ɑ��M����K�v������.
    Wire.write('A');

    Wire.endTransmission();

    // �擾�ҋ@(�ȈՔ�). �����Ƃ����Ƃ������l�͊撣���Ă�������.
    delay(10);

    // �R���p�X�Z���T����2�o�C�g�̃f�[�^��v��.
    Wire.requestFrom(compass_address, 2);

    // �f�[�^�擾�p�ϐ�
    byte heading_data[2] = {};
    int i = 0; // �J�E���^

    // Wire.available() �͌���Wiki�����Ă�������.
    while(Wire.available() && i < 2) {
        // �R���p�X�Z���T����1�o�C�g�̃f�[�^���擾
        // read()����1�o�C�g�������擾�ł��Ȃ��̂Ń��[�v��2��񂵂Ă���.
        heading_data[i] = Wire.read();
        ++i;
    }
    
    // 16bit(2byte)�̃f�[�^�̂����Aheading_data[0] �͏��8bit�A[1]�͉���8bit�̃f�[�^.
    return heading_data[0] * 256 + heading_data[1];
}


void setup() {
    Serial.begin(9600); // �f�o�b�O�p
    Wire.begin(); // �K���ĂԂ���

    delay(100); // �ꉞ�Z���T�[�쓮��ҋ@. �K�v�Ȃ�����

    // �L�����u���[�V�����̃^�C�~���O�͎��������ōl���邱��.
    calibration();
}


void loop() {
    int heading = read_compass();
    Serial.println(heading);
    delay(100);
}
