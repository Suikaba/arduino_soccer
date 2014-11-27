#include <Wire.h>

int compass_address = (0x42>>1);

void calibration() {
    // コンパスセンサーと通信を開始
    Wire.beginTransmission(compass_address);

    // キャリブレーション開始コマンド送信. 
    // 'C'がコマンドにあたる(チートシートに詳細あり).
    // 一度キャリブレーションコマンドを送信すると、停止コマンドを
    // 送信するまで永遠にキャリブレーションし続ける.
    // ただし、HMC5883Lとは違ってコマンド送信しても処理がとまることはない.
    // (つまり、裏で勝手にキャリブレーションしてくれてるということ)
    // 停止コマンドについては後述.
    Wire.write('C');

    // コンパスセンサーと通信切断
    Wire.endTransmission();

    // 15秒待機. 余裕をもってゆっくりと360度回転させられるような時間にする.
    delay(15000);

    Wire.beginTransmission(compass_address);

    // 停止コマンド送信
    Wire.write('E');

    Wire.endTransmission();
}


// 0 ~ 3600 で返ってくる(ex. 10度なら100)
int read_compass() {
    Wire.beginTransmission(compass_address);

    // データ計測コマンド送信.
    // 取得毎に送信する必要がある.
    Wire.write('A');

    Wire.endTransmission();

    // 取得待機(簡易版). もっとちゃんとしたい人は頑張ってください.
    delay(10);

    // コンパスセンサから2バイトのデータを要求.
    Wire.requestFrom(compass_address, 2);

    // データ取得用変数
    byte heading_data[2] = {};
    int i = 0; // カウンタ

    // Wire.available() は公式Wikiを見てください.
    while(Wire.available() && i < 2) {
        // コンパスセンサから1バイトのデータを取得
        // read()だと1バイトずつしか取得できないのでループを2回回している.
        heading_data[i] = Wire.read();
        ++i;
    }
    
    // 16bit(2byte)のデータのうち、heading_data[0] は上位8bit、[1]は下位8bitのデータ.
    return heading_data[0] * 256 + heading_data[1];
}


void setup() {
    Serial.begin(9600); // デバッグ用
    Wire.begin(); // 必ず呼ぶこと

    delay(100); // 一応センサー駆動を待機. 必要ないかも

    // キャリブレーションのタイミングは自分たちで考えること.
    calibration();
}


void loop() {
    int heading = read_compass();
    Serial.println(heading);
    delay(100);
}
