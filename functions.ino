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

int road (int error){
  integral=error+integral;//積分（誤差の足し算）
  //Serial.print(integral);
  //Serial.print(" ");
  if(integral>1000){ //1000以上であれば1000に
    integral=1000;
  }
  if(integral<-1000){  //-1000以下であれば-1000に固定
    integral=-1000;
  }
  deri=rotate-lasterror;//微分の値(前回との差)

  return error*k+integral*ki+deri*kd;//pid制御の値
}

Node* insert(int type, int direction, Node* preNode){
    Node *newNode;
    newNode = new Node;
    newNode->type = type;
    newNode->parent = preNode;
    newNode->left = NULL;
    newNode->right = NULL;
    if(direction == -1){
        preNode->left = newNode;
        newNode->direction = -1;
    }else{
        preNode->right = newNode;
        newNode->direction = 1;
    }
    return newNode;
}

void interchange(int type){

    if (reverse == -1)
    {
        if (curNode->parent == NULL)
        {
            return;
        }
        //Serial.println(direction);
        if (curNode->direction == -1)
        {
            if (curNode->parent->type == 1)
            {
                //Serial.println("return forward(L)");
                run();
                goalPath[pathIndex] = 5;
            }else{
                //Serial.println("return right(L)");
                R_run();
                goalPath[pathIndex] = 1;
            }
            curNode = curNode->parent;
        }else{
            if (curNode->parent->type == -1)
            {
                //Serial.println("return forward(R)");
                run();
                goalPath[pathIndex] = 5;
            }else{
                //Serial.println("return left(R)");
                L_run();
                goalPath[pathIndex] = 2;
            }
            curNode = curNode->parent;
        }
        pathIndex++;
        return;
    }

    //壁にあたったときの動作
    if(type == 5){
        reverse = 1;
        insert(5,direction,curNode);
        servoL.write(180);
        servoR.write(180);
        delay(rotateDelay);
        return;
    }

    //ゴールについたときの動作
    if(type == 7){
        reverse = -1;
        curNode = insert(7,direction,curNode);
        goal = curNode;
        servoL.write(180);
        servoR.write(180);
        delay(rotateDelay);
        return;
    }

    if(reverse == 0){
        curNode = insert(type,direction,curNode);
        direction = -1;
        if(type == -1){
            L_run();
        }else if(type == 0){
            L_run();
        }else{
            run();
        }
    }else{
        if(curNode->right == NULL){
            if(curNode->type == 0){
                run();
            }else{
                L_run();
            }
            reverse = 0;
            direction = 1;
            
        }else{
            curNode = curNode->parent;
            if(curNode->type == -1){
                run();
            }else{
                L_run();
            }
        }
    }
    return;
}

void makePath(){
    int j=0;
    String str="";
    for (int i = 20; i > -1; i--)
    {
        if (goalPath[i] != 0)
        {
            finalPath[j] = goalPath[i];
            j++;
        }
    }
    
    for (int i = 1; i < j; i++)
    {
        //Serial.print(finalPath[i]);
        str += String(finalPath[i]);
    }
    Serial.println(str);
    return;
}
