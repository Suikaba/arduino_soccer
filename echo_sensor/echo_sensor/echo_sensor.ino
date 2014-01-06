#include <Wire.h>

#define ECHO_NUM 4
#define SELF_ADDRESS 0x16

int pin[ECHO_NUM] = {9, 10, 11, 12}; // front back left right
int ret[ECHO_NUM] = {};

int UsonicMeasureRead(int pin);

void request_event() {
  char data[4] = {};
  for(int i=0; i<4; ++i) {
    data[i] = (char)ret[i];
    Serial.print((byte)data[i]); Serial.print(" ");
  }
  Wire.write(data);
  Serial.println();
}

void setup() {
  Wire.begin(SELF_ADDRESS);
  Wire.onRequest(request_event);
  Serial.begin(9600);
}

void loop() {
  for(int i=0; i<ECHO_NUM; ++i) {
    int tmp = UsonicMeasureRead(pin[i]);
    if(tmp == -1 || tmp > 255) {
      ret[i] = 255;
    } else {
      ret[i] = tmp;
    }
  }
  delay(50);
}

// =======================================================================
// read ping sensor
// return: distance(cm)
// =======================================================================
int UsonicMeasureRead(int pin){
    // have ping sensor output pulse for 5 micro second
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delayMicroseconds(2);
    digitalWrite(pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
    int ans = -1;
    long time = pulseIn(pin, HIGH);
    if (time < 18000) {             // ignore the distance more than three metor.
         ans = (time / 29) / 2;     // divide the distance in two
                                    // because pulse make a round trip.
    }
    return ans;
}
