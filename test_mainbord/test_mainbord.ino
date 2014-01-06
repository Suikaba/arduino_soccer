//RCJ用のメインボード性能チェック用

/*
//test1,serial通信=ok

void setup(){
  Serial.begin(9600);
}

void loop(){
  if ( Serial.available() ){
    Serial.read();
    Serial.print("hellow world");
  }
  delay(100);
}
*/

/*
//test2,led(49,51,53)=ok
//同時に点灯
int pin[] = {49,51,53}; //ledのピン番号
void setup(){
  for (int i = 0 ; i < 3 ;i++){
    pinMode(pin[i],OUTPUT);
  }
}

void loop(){
  for (int j = 0 ; j < 3 ; j++ ){
    digitalWrite(pin[j],HIGH);
  }
  delay(1000);
  for (int k = 0 ; k < 3 ; k++ ){
    digitalWrite(pin[k],LOW);
  }
  delay(1000);
}

//一つだけ点灯
int pin = 53; //ピン番号
void setup(){
  pinMode(pin,OUTPUT);
}

void loop(){
  digitalWrite(pin,HIGH);
  delay(1000);
  digitalWrite(pin,LOW);
  delay(1000);
}
*/

/*
//test3,resetswitch=ok
//リセットスイッチを押せば一回だけ文字が送られる
void setup(){
  Serial.begin(9600);
  Serial.println("Hello World");
}

void loop(){}
*/

/*
//test4,switch(50,48)=ok
//スイッチがオンならledが点く
int pin[] = {49,51,53}; //ledのピン番号
void setup(){
  for (int i = 0 ; i < 3 ;i++){
    pinMode(pin[i],OUTPUT);
  }
}

void loop(){
  int a = digitalRead(48);
  int b = digitalRead(50);
  if (a == 1){
    digitalWrite(pin[1],HIGH);
  }else{
    digitalWrite(pin[1],LOW);
  }
  if (b == 1){
    digitalWrite(pin[0],HIGH);
  }else{
    digitalWrite(pin[0],LOW);
  }
}
*/

/*
//test5,I2C通信=ok
//コンパスセンサを使うプログラム（ばいsuibaka)

#include <Wire.h>

int address = 0x42 >> 1;

void setup()
{
  Serial.begin( 9600 );
  Wire.begin();
}

void loop()
{
  Wire.beginTransmission( address );
  Wire.write( "A" );
  Wire.endTransmission();
  delay( 10 );
  Serial.println( read_data() );
  delay( 100 );
}

// 0 ~ 3600 で返る
// 返答がなかったら -1 が返る
int read_data()
{
  Wire.requestFrom( address, 2 );
  byte headingData[2] = {};
  int i = 0;
  while(Wire.available() && i < 2)
  { 
    headingData[i] = Wire.read();
    i++;
  }
  return headingData[0] * 256 + headingData[1];
}
*/

//test6,analogデータ=ok (0,1,2,5,6,7,10,11,12,13,14,15)
//デジタルの場合(54,55,56,59,60,61,64,65,66,67,68,69)
/*
//超音波センサーが読み込めるか（ばいsuparobo改)=ok

#define pingPin1 54// センサー接続のピン番号
void setup() {
  Serial.begin(9600) ;   // 9600bpsでシリアル通信のポートを開きます
}
void loop() {
     int cm1,cm2 ;
     cm1 = UsonicMeasurRead(pingPin1) ;   // センサーから距離を調べる
     Serial.print(cm1) ;                 // 距離を表示する
     Serial.println("cm\n") ;
     delay(1000) ;
}
// 超音波センサーから距離を得る処理
// pin=センサーに接続しているピン番号
// 距離をｃｍで返す(検出できない場合は０を返す)
int UsonicMeasurRead(int pin){
     long t ;
     int ans ;     
// 超音波センサーに5usのパルスを出力する
     pinMode(pin, OUTPUT) ;    // ピンを出力モードにする
     digitalWrite(pin, LOW) ;
     delayMicroseconds(2) ;
     digitalWrite(pin, HIGH) ;
     delayMicroseconds(2) ;
     digitalWrite(pin, LOW) ;
     pinMode(pin, INPUT) ;              // ピンを入力モードにする
     t = pulseIn(pin, HIGH) ;           // パルス幅の時間を測る
     if (t < 18000) {                   // ３ｍ以上の距離は計算しない
          ans = (t / 29) / 2 ;          // 往復なので２で割る
     } else ans = 0 ;
     return ans ;
}
*/

//普通のアナログのデータが読み込めるか=ok
//使わないポートはデジタル出力でLOWレベルにしておくこと
int unuseport[] = {0,1,2,5,6,7,10,11,12,13};   //使わないポート番号
int useport[] = {14,15} ;    //使いたいポートの番号
int num1 = 10 ;   //使わないポートの要素数
int num2 = 2  ;   //使いたいポートの要素数
unsigned long val = 0 ;  //値格納用スペース

void setup(){
  Serial.begin(9600);
  for (int h = 0 ; h < num1 ; h++ ){
    pinMode( unuseport[h] + 54 ,OUTPUT );
  }
}

void loop(){
  for (int i = 0; i < num1 ; i++){
    digitalWrite(unuseport[i] + 54 , LOW );
  }
  for (int j = 0; j < num2 ; j++ ){
    val = analogRead(useport[j]) ;
    val = val * 100 / 1023 ;
    Serial.print(val) ;
    Serial.print("% ") ;
  }
  Serial.println();
  Serial.println();
  delay(1000);
}

//テスト完了。
//すべての機能は正常に使えます。
/*ほかにも
割り込み　＝　そのピンがHIGH→LOWか、LOW→HIGHに替わると
　　　　　　　プログラムの予定されていた変数が始まる
シリアル通信　＝　対パソコン用（既に使用中）以外にもたくさんのポートが使える
SPI通信　＝　arduino同士の通信はできないが高速での通信ができる
参照電圧　＝　参照電圧ピンにかかっている電圧を基準(＝1024)とできる。
などの機能があります。
*/




  
