/*IRセンサーを直接読み取るプログラム(IRM3638)*/
//結果；距離は検出できそう（5段階くらい？）
//→うまくいけばボールの位置を判定できるかも

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
}

void loop() {
  unsigned long t = pulseIn(2, LOW);
  Serial.println(t);
  delay(100);
}
