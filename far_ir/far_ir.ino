#include <Wire.h>

const int  PIN_NUM = 9;

int far_ir_pin[PIN_NUM] = {4, 5, 6, 7, 8, 9, 10, 11, 12};
int pin_value[PIN_NUM] = {0};

int find_max_ir() {
    int max_value = 0;
    int max_pin = 0;
    for(int i=0; i<PIN_NUM; ++i) {
        if(pin_value[i] > max_value) {
            max_value = pin_value[i];
            max_pin = i; // return value is actual pin number - 2
        }
    }
    return max_pin;
}


//memo status
// 0:,300)
// 1:[300,350)
// 2:[350,400)
// 3:[400,450)
// 4:[450,500)
// 5:[500,540)
// 6:[540,
void request_event() {
    int max_ir = find_max_ir();
    byte send_value = 0;
    send_value = max_ir*10;
    if(pin_value[max_ir] > 540) {
        send_value += 6;
    } else if(pin_value[max_ir] > 500) {
        send_value += 5;
    } else if(pin_value[max_ir] > 450) {
        send_value += 4;
    } else if(pin_value[max_ir] > 400) {
        send_value += 3;
    } else if(pin_value[max_ir] > 350) {
        send_value += 2;
    } else if(pin_value[max_ir] > 300) {
        send_value += 1;
    }
    //Serial.print(send_value[0]); Serial.print(" ");
    //Serial.println(send_value[1]);
    Wire.write(send_value);
}

void setup() {
    Serial.begin(9600);
    for(int i=0; i<PIN_NUM; ++i) {
        pinMode(far_ir_pin[i],INPUT);
    }
    Wire.begin(0x10);
    Wire.onRequest(request_event);
}

void loop() {
    delay(20);
    for(int i=0; i<PIN_NUM; ++i) {
        pin_value[i] = pulseIn(far_ir_pin[i], LOW, 1500);
        //Serial.print(pin_value[i]); 
        //Serial.print(" ");
    }
    //Serial.println();
}
