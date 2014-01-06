//センサーモニター用プログラム

#include <Wire.h>

int ping_value[4] = {0};   //見たいポート番号

void read_ping() {
  Wire.requestFrom(0x16, 4);
  int i=0;
  while(Wire.available() && i < 4) {
    ping_value[i] = Wire.read();
    Serial.print(ping_value[i]); Serial.print(" ");
  }
  Serial.println();
  //Wire.endTransmission();
}

void setup(){
  Wire.begin();
  Serial.begin(9600);
}

void loop(){
  read_ping();
  delay(100);
}


