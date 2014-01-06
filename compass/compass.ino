
#include <Wire.h>

int address = (0x42>>1);

void calibration() {
  //キャリブレーション開始コマンド送信
  Serial.println("hog");
  Wire.beginTransmission(address);
  Wire.write('C');
  Wire.endTransmission();
  Serial.println("C");
  //30秒ほど待つ（6秒〜3分まで）
  //この間に数回モジュールを回転させる
  delay(30000);

  //キャリブレーション終了コマンド送信
  Wire.beginTransmission(address);
  Wire.write('E');
  Wire.endTransmission();
  Serial.println("E");
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  delay(100);
  calibration();
  delay(100);
}

void loop()
{
  Wire.beginTransmission(address);
  Wire.write('A');
  Wire.endTransmission();
  delay(10);
  read_data();
  delay(100);
}

// 0 ~ 3600 で返る
int read_data()
{
  Wire.requestFrom(address, 2);
  byte headingData[2] = {};
  int i = 0;
  while(Wire.available() && i < 2)
  {
    headingData[i] = Wire.read();
    i++;
  }
  int test = headingData[0]*256 + headingData[1];
  Serial.println(test);
  return test;
}

