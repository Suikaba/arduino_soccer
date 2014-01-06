/*遠距離用IRセンサのテスト*/

void setup(){
  Serial.begin(9600);
}

void loop(){
  int IR = analogRead(2);
  Serial.println (IR);
  delay(200);
}
