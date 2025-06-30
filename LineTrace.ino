
#include <Adafruit_MCP3008.h>
#include <Servo.h>
Adafruit_MCP3008 adc; //下部センサの定義 
Servo servoR;
Servo servoL;
int v[6]={0,0,0,0,0,0}; //6連センサの値を格納する配列
int b=400;//黒線上であるかとないかの閾値を仮に400とする
int rotate;
int integral =0; //integralの初期値
int lasterror = 0;
int deri =0;//微分するための値格納
float k = 0.056; //PID制御の値
float ki =0.05;//Iの係数(0.005仮定)
float kd= 0.1;//Dの係数(0.1仮定)
int speedError = 25;
void setup() {
servoR.attach(4);//右車輪のモータのピンが4に配線されている場合のアタッチ
servoL.attach(5);//○○○.(i)でi番目のピンのモータを関連づける

/*モーター初期化*/
//引数に90を入れると基本的には車輪のモータは停止する
servoR.write(90); //○○○.write(int)でモータの回転角（速度）変える
servoL.write(90); //値は0～180の間

adc.begin(); //センサを初期化
Serial.begin(9600); //シリアル通信の転送速度
}

//メインループ
void loop(){
  //センサの値を取得
  rotate = read();
  //センサの値をモニターに出力
  Serial.print(rotate);
  Serial.print(" ");

  //pidの値取得
  int pid=road(rotate);
  //走行
  runRotate(pid);
  Serial.print(pid);
  Serial.println();
  lasterror=rotate;//前回の値を保持

  //分岐判定
  int* flag = readSide();
  if (flag[0] == 1 || flag[1] == 1){
    servoL.write(180);
    servoR.write(0);
    delay(50);
  }
  flag = readSide();
  if (flag[0] == 1 && flag[1] == 1){
    //もし両サイドのセンサが反応したら
    L_run();
  }else if(flag[0] == 0 && flag[1] == 1){
    // もし右だけ反応したら
    servoL.write(180);
    servoR.write(0);
    delay(500);
  }else if(flag[0] == 1 && flag[1] == 0){
    L_run();
  }

  //壁判定
  wall();

  //ゴール判定
  goal();
}

void R_run(){
  servoL.write(180);
  servoR.write(95);
  delay(1100);
}

void L_run(){
  servoL.write(85);
  servoR.write(0);
  delay(1100);
}

//入力が大きい値であれば大きく曲がる関数
void runRotate(float angle){
  if (angle>0){
    /*L 180→180 R 0→180*/
    servoL.write(180-speedError);
    servoR.write(int(angle));
  }else{
    /*L 180→0 R 0→0*/
    servoL.write(180+int(angle)-speedError);
    servoR.write(0);
  }
}

void run(){
  servoL.write(180);
  servoR.write(0);
  delay(100);
}

//センサの値を読んで変換する関数
int read(){
  int rotate = 0;
  //センサの値を格納
  for (int i=0;i<6;i++) {v[i] = adc.readADC(i);}

  //各センサに重みをかける
  //v[0] *= 0;
  v[1] *= -1.3;
  v[2] *= -1;
  v[3] *= 1;
  v[4] *= 1.3;
  //v[5] *= 0;
  
  //各センサの値を足し合わせる
  for (int i=0;i<6;i++){
    rotate += v[i];
  }
  
  return rotate;
}

int* readSide(){
  static int flag[2] = {0,0};
  v[0] = adc.readADC(0);
  v[5] = adc.readADC(5);

  if (v[0] > 200){
    flag[0] = 1;
  }else{
    flag[0] = 0;
  }

  if (v[5] > 200){
    flag[1] = 1;
  }else{
    flag[1] = 0;
  }
  return  flag;
}

void wall(){
  Serial.print(analogRead(A5));
  Serial.print(" ");
  if (analogRead(A5) > 550){
    servoL.write(180);
    servoR.write(180);
    delay(1550);
  }
}

void goal(){
  int flag = 0;
  for(int i=1; i<5; i++){
    if(adc.readADC(i) < 400){
        flag += 1;
    }
  }
  
  if(flag == 4){
    servoL.write(180);
    servoR.write(180);
    delay(100000);
  }
}

int road (int error){
  integral=error+integral;//積分（誤差の足し算）
  Serial.print(integral);
  Serial.print(" ");
  if(integral>1000){ //1000以上であれば1000に
    integral=1000;
  }
  if(integral<-1000){  //-1000以下であれば-1000に固定
    integral=-1000;
  }
  deri=rotate-lasterror;//微分の値(前回との差)

  return error*k+integral*ki+deri*kd;//pid制御の値
}
