/*近距離用IRセンサのテスト*/
#define NEAR_IR_NUM  8

int near_ir_pin[NEAR_IR_NUM] = {0, 1, 2, 3, 4, 5, 6, 7};

void setup(){
  Serial.begin(9600);
}

void loop(){
  for(int i=0; i<NEAR_IR_NUM; ++i) {
    unsigned long val = analogRead(near_ir_pin[i]);
    val = val * 100 /1023 ;
    Serial.print(val); Serial.print(" ");
  }
  Serial.println();
  delay(100);
}
