//
// コンパスセンサー HMC5883L のExample.
// ライブラリの内部実装の詳細は知らなくても使えるようにしてます.
//
#include <Wire.h>
#include <HMC5883L.h>

// コンパス本体
HMC5883L compass;
// エラー確認用変数
int error = 0;

void setup() {
    Serial.begin(9600); // Serialはテストで出力するなら必要
    Wire.begin();       // この行は必ず必要

    // 以下printlnの行はすべてデバッグ用出力
    Serial.println("Constructing new HMC5883L");
    
    compass = HMC5883L();

    // スケールの設定. 1.3fで設定する.
    Serial.println("Setting scale to +/- 1.3 Ga.");
    error = compass.set_scale(1.3f); // エラーでなければ0が返ってくる
    if(error != 0) {
        // エラーの詳細を出力
        Serial.println(compass.get_error_text(error));
    }

    // 測定モードを設定する.
    // MESUREMENT_CONTINUOUS: 継続測定モード. 
    //                        関数を呼び出さなくても裏で勝手にデータを取ってくれてる.
    // MEASUREMENT_SINGLESHOT: 単発測定. 関数を呼んだタイミングで測定開始.
    // MEASUREMENT_IDLE: よくわからない. 詳細はチートシート見てください.
    // 
    // CONTINUOUSモードにしておきましょう.
    Serial.println("Setting measurement mode to continous.");
    error = compass.set_measurement_mode(MEASUREMENT_CONTINUOUS);
    if(error != 0) {
        Serial.println(compass.get_error_text(error));
    }

    delay(50); // 念のためセンサー類の起動待機. 必要ないかも


    // キャリブレーション.
    // calibrateは、呼び出すとキャリブレーションが終わるまでここで処理が止まる.
    // キャリブレーション中に、センサーを360度ゆっくりと回転させること.
    // できるだけ水平な場所で行う(傾けたりしてはならない).
    compass.calibrate(15); // 15秒間キャリブレーションを行う.
}

void loop() {
    // 生データの取得. 普通は使うことはないと思うのであまり気にしなくてもいい.
    magnetometer_raw raw = compass.read_raw_axis();

    //  生データをスケール処理したあとのデータの取得. これも使わないと思う.
    magnetometer_scaled scaled = compass.read_scaled_axis();

    // 今向いている方向の角度を取得. 弧度法(ラジアン)ではなく度数法(0 ~ 359.9..).
    // 本番プログラムで使うことになるのはこの関数だと思う.
    float heading_degrees = compass.read_heading();
    // ラジアンに変換したいなら以下のようにする.
    float heading_rad = heading_degrees * PI / 180;

    // デバッグ出力
    output(raw, scaled, heading_rad, heading_degrees);

    // ここのディレイの値はは他のセンサーとも相談して決めてください.
    delay(30);
}


// デバッグ出力用.
void output(magnetometer_raw raw, magnetometer_scaled scaled, float heading, float heading_degrees)
{
   Serial.print("Raw: ");
   Serial.print(raw.x_axis);
   Serial.print(" ");   
   Serial.print(raw.y_axis);
   Serial.print(" ");   
   Serial.print(raw.z_axis);
   Serial.print("   Scaled: ");
   
   Serial.print(scaled.x_axis);
   Serial.print(" ");   
   Serial.print(scaled.y_axis);
   Serial.print(" ");   
   Serial.print(scaled.z_axis);

   Serial.print("   Heading: ");
   Serial.print(heading);
   Serial.print(" Radians. ");
   Serial.print(heading_degrees);
   Serial.println(" Degrees.");
}
