#define IN1 4
#define IN2 3
#define ENA 2
#define IN3 6
#define IN4 5
#define ENB 7

#define BLOCK1 Map[8][1];
#define BLOCK2 Map[8][4];
#define BLOCK3 Map[8][8];
#define BLOCK4 Map[5][8];
#define BLOCK5 Map[1][5];
#define BLOCK6 Map[1][8];

#define TURN_SPEED 100

int Map[10][10];//10*10场地
int x=0,y=2;//(0,2)
int direct[4][4]={//场地俯视图上1下2左3右4，前后左右.Turn left：direct[now_direction][2] ; Turn right:direct[now_direction][3];
  0,1,2,3,
  1,0,3,2,
  2,3,1,0,
  3,2,0,1
};
int movx[4]={  0,0,-1,1 },movy[4]={ 1,-1,0,0};
int NowDirection = 4 ;
int s[8];
int SumLines=0;
int choice=1;//1 go straight ; 2 turn left ; 3 turn right
double k,vl,vr,kl,kr;
bool isTurning;
bool isPassingLine;
bool isToFollowLine;
bool isFirstTime;

void LeftMotor(int speed);
void RightMotor(int speed);
void TurnLeft(int speed);
void TurnRight(int speed);
void FollowLine();
void Move();//choice 全局变量
void Count();
void data();

void AllStop();
void Adjustment();
void ReadSensors();
void LocataBlock();
void DistanceTime();

void setup()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  Serial.begin(9600);
  kl=0.02;
  kr=-0.02;
  vl=200;
  vr=165;
  isTurning = false;
  isPassingLine = false;
  isToFollowLine = true;
}

void loop()  
{
  data();
  Count();
  Move();
}

void data()
{
  // In contest white: 0. black: 1 or 1024
  // white-black conversion, only used in test
  // i.e., white: 1, black 0
  s[0]=1024-analogRead(A2);
  s[1]=1024-analogRead(A3);
  s[2]=1024-analogRead(A4);
  s[3]=1024-analogRead(A5);
  s[4]=1024-analogRead(A0);
  s[5]=1024-analogRead(A1);
  s[6]=1024-analogRead(A6);
  s[7]=1024-analogRead(A7);

  // filter out environment light: convert to digital
  for(int i = 0; i < 8; i++){
   if(s[i]>500) {
    // detects white area
    s[i] = 1024;
   } else {
    // detects black area
    s[i] = 0;  
  }
 }  
}

//计数，定位？
void Count()
{
   static int flag = 0 ;
   if( choice == 1 )
   {
    if(!s[4]&&!s[6])
    {
      flag=1;
    }
    if(!s[5]&&!s[7]&&flag)
    {
      SumLines++;
      x+=movx[NowDirection];
      y+=movy[NowDirection];
      flag=0;
    } 
  }
}


void Move()
{
  //Go straight 1 ;turn left 2;turn right 3
  // if one of the side sensors detects the crossing line(black in test, white in contest)
  if(!s[6] && !s[4] && ( choice != 1 )) {
    isToFollowLine = false;
    isPassingLine = true;
  }
  
  if(isToFollowLine){
    FollowLine();
  } else {
    if( choice == 2 )
    {
       TurnLeft(TURN_SPEED);
    // if is turning and front sensors pass black totally
       isTurning=true;
      // if s[0] detects black, it leaves the black line.
       if(!s[0]) {
       // front sensors detect white area
        isTurning = true;
        isPassingLine = false;
        
      }

      if(isTurning) {
      // if one of the front sensors  dectects black, switch back to follow line, a turning succeeds
      
      //isToFollowLine = isToFollowLine || !s[i];
      if(!s[3]) {
        isToFollowLine = true;
        isTurning = isPassingLine = false;
        choice = 1;
        NowDirection=direct[NowDirection][2];
       }
     }
     
   }
    if( choice == 3 )
    {
       TurnRight(TURN_SPEED);
    // if is turning and front sensors pass black totally
      isTurning=true;
      // if s[3] detects black, it leaves the black line.
      if(!s[3]) {
       // front sensors detect white area
      isTurning = true;
      isPassingLine = false;
     }

     if(isTurning) {
      // if one of the front sensors  dectects black, switch back to follow line, a turning succeeds
      
      //isToFollowLine = isToFollowLine || !s[i];
      if(!s[0]) {
        isToFollowLine = true;
        isTurning = isPassingLine = false;
        choice = 1;
        NowDirection=direct[NowDirection][3];
       }
     }  
    }
  }
}

/*void LocateBlock()
{
  //judge from distance at (8,2)and(8,8)
}*/

/*void DistanceTime(void)
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);//发送10μS的高电平触发信号
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  distance = pulseIn(EchoPin, HIGH); // 检测脉冲宽度，即为超声波往返时间
}*/

void LeftMotor(int speed)
{
  if(speed>255)speed=255;
  
  if(speed > 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  }
  
  else 
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2,HIGH );
    analogWrite(ENA, -speed);
  }
/*  if(speed < 0)
 *   else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }*/
}

void RightMotor(int speed)
{
  if(speed>255)speed=255;
  if(speed > 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  }
  else 
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  }/*
  if(speed < 0)
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }*/
}

void TurnLeft(int speed) 
{
  LeftMotor(-speed);
   RightMotor(speed);
}

void TurnRight(int speed) 
{
  TurnLeft(-speed);
}

void FollowLine()
{
  int x=(-3)*s[0]+(-1)*s[1]+1*s[2]+3*s[3];
  double leftSpeed = vl+kl*x;
  double rightSpeed = vr+kr*x;

  LeftMotor(leftSpeed);
  RightMotor(rightSpeed);
}
