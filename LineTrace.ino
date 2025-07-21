
#include <Adafruit_MCP3008.h>
#include <Servo.h>
#include <new.h>

//ノードの宣言
struct Node
{
    int type; //スタートから見て　左分岐→-1　T字路→0　右分岐→1　スタート→-10　行き止まり→5　ゴール→7
    int direction; //親から見て　左→-1　右→1
    Node *parent, *left, *right;
};

Node *root, *curNode, *goal;
int direction = -1; //親から見て　左→-1　右→1
int reverse = 0; //葉に向かって進む→0　根に向かって進む→1　ゴール発見後根に戻る→-1
int goalPath[20]; //ゴールからスタートまでの道筋　左折→-1　直進→5　右折→1
int finalPath[20]; //スタートからゴールまでの道筋　左折→-1　直進→5　右折→1
int pathIndex = 0;

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
int speedError = 25;//
int delays = 1000;//曲がるときの秒数
int rotateDelay = 1600;//Uターンするときの秒数
char com = "b";

void setup() {
  servoR.attach(4);//右車輪のモータのピンが4に配線されている場合のアタッチ
  servoL.attach(5);//○○○.(i)でi番目のピンのモータを関連づける

  //モーター初期化
  servoR.write(90); 
  servoL.write(90); //値は0～180の間

  adc.begin(); //センサを初期化
  Serial.begin(9600); //シリアル通信の転送速度

  //スタートノードの作成
  root = new Node;
  root->type = -10;
  root->parent = NULL;
  curNode = root;
}

//メインループ
void loop(){
  //PCからの命令を待機
  while(1){
    if(com == 'a'){
      break;
    }
    servoR.write(90); //○○○.write(int)でモータの回転角（速度）変える
    servoL.write(90); //値は0～180の間
    com = Serial.read();
    delay(100);
  }
  //センサの値を取得
  rotate = read();
  //pidの値取得
  int pid=road(rotate);
  //走行
  runRotate(pid);
  lasterror=rotate;//前回の値を保持

  //分岐判定
  int* flag = readSide();
  if (flag[0] == 1 || flag[1] == 1){
    servoL.write(180);
    servoR.write(0);
    delay(20);
    //再判定
    flag = readSide();
    if(flag[0] == 1 || flag[1] == 1){
      if(reverse == -1){
        //もしゴール到達後なら
        interchange(-1);
      }else if(reverse == 1){
        //もしバックトラック中なら
        //Serial.println("Back");
        interchange(1);
      }else if (flag[0] == 1 && flag[1] == 1){
        //もし両サイドのセンサが反応したらT字路
        interchange(0);
      }else if(flag[0] == 0 && flag[1] == 1){
        // もし右だけ反応したら右分岐
        interchange(1);
      }else if(flag[0] == 1 && flag[1] == 0){
        //もし左だけ反応したら左分岐
        interchange(-1);
      }
    }
  }

  //壁判定
  wall();

  //ゴール判定
  goals();
}

//右折する関数
void R_run(){
  servoL.write(180);
  servoR.write(102);
  delay(delays);
  run();
}

//左折する関数
void L_run(){
  servoL.write(85);
  servoR.write(0);
  delay(delays);
}

//入力値に応じて回転半径を変える関数(ライントレース用)
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

//直進する関数
void run(){
  servoL.write(180);
  servoR.write(0);
  delay(500);
}

//ゴールを検出する関数
void goals(){
  if (analogRead(A5) > 550){
    interchange(7);
  }
}

//行き止まりを検出する関数
void wall(){
　//検出部
  int flag_wall = 0;
  for(int i=1; i<5; i++){
    if(adc.readADC(i) < 200){
        flag_wall += 1;
    } 
  }
    
  if(flag_wall == 4){
  //もし行き止まりであれば
    if(reverse == -1){
    　//もしスタートンに到達したら
      makePath();
      servoR.write(90);
      servoL.write(90);
      delay(100000);
      return;
    }
    interchange(5);
  }
}




