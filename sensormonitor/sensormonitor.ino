//センサーモニター(アナログ用)
#define num1 8
#define num2 8

//int port[] = {0,1,2,5,6,7,10,11,12,13,14,15};    //アナログのポート番号
int unuseport[num1] = {9,10,11,12,13,14,15,16};   //使わないポート番号
int useport[num2] = {0,1,2,3,4,5,6,7} ;    //使いたいポートの番号
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
    
