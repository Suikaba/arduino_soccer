#include <Wire.h>

const int PING_ADDRESS = 0x16;

void setup() {
    Wire.begin();
}

void loop() {
    int ping_value[4] = {0};
    Wire.requestFrom(PING_ADDRESS, 4);
    int i=0;
    while(Wire.available() && i < 4) {
        ping_value[i] = Wire.read();
        ++i;
    }
    Wire.endTransmission();
    delay(20);
}
