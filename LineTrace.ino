//ライントレース用下部センサ読み取り sample
/*黒線検知したら直進やめて停止するプログラム*/
#include <Adafruit_MCP3008.h>
/*上記ヘッダファイルをインクルードをしないと6連センサを使用できない
  ツール→ライブラリのインクルード→ライブラリの管理から
  ネットワーク経由出インストール，あるいはWebClassのzipをダウンロードして
  Adafruit_MCP3008をインストールする必要がある*/
#include <Servo.h>
Adafruit_MCP3008 adc; //下部センサの定義 
Servo servoR;
Servo servoL;
int v[6]={0,0,0,0,0,0}; //6連センサの値を格納する配列
int b=400;//黒線上であるかとないかの閾値を仮に400とする
int mtrL=0, mtrR=0;
int i;
int rotate;
void setup() {
  // put your setup code here, to run once:
servoR.attach(4);//右車輪のモータのピンが4に配線されている場合のアタッチ
servoL.attach(5);//○○○.(i)でi番目のピンのモータを関連づける

/*モーター初期化*/
//引数に90を入れると基本的には車輪のモータは停止する
servoR.write(90); //○○○.write(int)でモータの回転角（速度）変える
servoL.write(90); //値は0～180の間

adc.begin(); //センサを初期化
Serial.begin(9600); //シリアル通信の転送速度

}
void loop(){
  rotate = read();
  Serial.print(rotate);
  Serial.print(" ");
  runRotate(rotate*0.056);
  Serial.print(rotate*0.056);
  Serial.println();
}

void R_run(){
  servoL.write(180);
  servoR.write(90);
  delay(100);
}

void L_run(){
  servoL.write(90);
  servoR.write(0);
  delay(100);
}

void runRotate(float angle){
  if (angle>0){
    /*L 180→180 R 0→180*/
    servoL.write(180);
    servoR.write(int(angle));
  }else{
    /*L 180→0 R 0→0*/
    servoL.write(180+int(angle));
    servoR.write(0);
  }
}

void run(){
  servoL.write(180);
  servoR.write(0);
  delay(100);
}

int read(){
  int rotate = 0;
  /*
  for (i=0;i<6;i++){ //6センサ分、端のセンサから1つずつセンサ値読み取り
    if(adc.readADC(i) > b){
      v[i] = 1;
    }else{
      v[i] = 0;
    }
  }*/
  for (i=0;i<6;i++) {v[i] = adc.readADC(i);}
  v[0] *= -2;
  v[1] *= -1;
  v[2] *= -1;
  v[3] *= 1;
  v[4] *= 1;
  v[5] *= 2;
  
  for (i=0;i<6;i++){
    rotate += v[i];
  }
  return rotate;
}


