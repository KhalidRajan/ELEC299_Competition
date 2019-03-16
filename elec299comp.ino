//ELEC 299 competitions
//for teams 10 11 12

//libraries to include
#include <Servo.h>
#include "QSerial.h"

//pin definitions
#define IRPIN 3

#define IRFRONT A1
#define GRIPSENSOR A2
#define L1 A3
#define L2 A4
#define L3 A5
#define RIGHTSPD 6
#define RIGHTDIR 7
#define LEFTSPD 5
#define LEFTDIR 4
#define MAXPULSE 65000
#define RESOLUTION 20
#define EL 8
#define EL 9
//other constant definitions
#define threshold1 225
#define threshold2 350

//typedefs and structs
typedef struct specloc
{
  int x;
  int y;
  int dir;
}specLoc;   

//start of global variables
Servo PAN, TILT, GRIP;

QSerial IRrx;

boolean running = true;
boolean debugging = false;

int x = 0;
int y = 0;
int dir = 0;
int instructionnumber = 0;//make this one the same all around
int dist;
int force = 0;

int pathselect;

//end of global variables

//function definition area
void drStraight();
void pivot(int, int);
void drive(bool);
void driveTo(specLoc tLoc);
//void rev();
void stopD();
//void updateLoc();
bool makeReady(specLoc targetL,specLoc currentL);


//VARIABLES IMPORTED FROM OTHER PROJECTS,WHEN POSSIBLE MERGE WITH OTHER VARIABLES
//path tracking variables
int E1 = RIGHTSPD;//right Wheel Speed
int E2 = LEFTSPD;//left Wheel speed
int M1 = RIGHTDIR;//right(forward HIGH)or back LOW
int M2 = LEFTDIR;//left(forward HIGH)or back LOW
int L = L1;
int R = L2;
int C = L3;
int countInter;

int LCount = 0;
int RCount = 0;

int LTHRESH = 900; //black
int RTHRESH = 900; //black
int CTHRESH = 900; //black
int lval;
int rval;
int cval;
float velFact = 1.2;
float velRW = 100*velFact;
float velLW = 90*velFact;
specLoc currentLoc;
specLoc targetLoc[14];
specLoc homeLocS[3] = {{1,0,2},
                      {2,0,2},
                      {3,0,2}};
specLoc homeLocN[3] = {{1,0,1},
                    {2,0,1},
                    {3,0,1}};
//IR variables
byte startIR = '0'; //'0' for check sensor for byte, '1', '2', '3' to force without checking.

//start of setup code
void setup() {
  
Serial.begin(115200);
pinMode(L1,OUTPUT);
pinMode(L2,OUTPUT);
pinMode(L3,OUTPUT);
if(startIR = '0')
  startIR = IRreceive();
//check for received or forced startIR value
switch(startIR)
{
  case '1':
  pathselect = 0;
  break;
  case '2':
  pathselect = 1;
  break;
  case '3':
  pathselect = 2;
  break;
  default:
  Serial.println("Error. IR received wrong or startIR was not '0', '1', '2', '3'");
  pathselect = -1;
}

pinMode(GRIPSENSOR, INPUT); //gripforcesensor

//motor initialization code area
  PAN.attach(11);    //pan fully left at 0, fullly right at 180, 90 is center
  TILT.attach(9);    //tilt horizontal at 70 degrees, verticle at 160, dont go below 15 degrees
  GRIP.attach(10);   //grip open entirely at 40 degrees to closed entirely at 180 degrees
  PAN.write(90);
  delay(100);
  TILT.write(160);
  delay(100);
  GRIP.write(40);
  delay(100);
  
  
  //test path finding initialization code. Please update later.
  currentLoc = {pathselect+1,-1,0};

  if (currentLoc.x == 1){
      driveTo({1,0,0});
      Serial.println("done: driveTo({1,0,0}) ");
  }else if (currentLoc.x == 2){
      driveTo({2,0,0});
  }else {
      driveTo({3,0,0});
  }
      
      driveTo({3,0,1});
      
      driveTo({0,0,3});
      
      driveTo({0,4,0});
     
      driveTo({1,4,1});

      driveTo({1,0,2});

//delay before starting loop.
delay(1000);
}






void loop() {
  serialCheck();
  
  //test code for get and drop only
  getBall();
  delay(1000);
  dropBall();
  delay(1000);
  //line follower code()
  
}



void serialEvent(){
  debugging = true;
  while(Serial.available()){
    delay(400);
    char rcv = (char)Serial.read();
    switch (rcv){
      //Right turn
      case 'R':
      Serial.println("MOO: Turning Right");
      break; //have a kitkat

      //Left turn
      case 'L':
      Serial.println("MOO: Turning Left");
      break;

      //Start or stop toggle
      case 'S':
      if(running)
        Serial.println("MOO: Stopping");
      else
        Serial.println("MOO: Starting");
      running = !running;
      break;

      //Increment X position variable
      case 'X':
      Serial.print("MOO: Adjust x, now: ");
      x = (x+1)%5;
      Serial.println(x);
      break;

      //Increment Y position variable
      case 'Y':
      Serial.print("MOO: Adjust y, now: ");
      y = (y+1)%6;
      Serial.println(y);
      break;

      //Increment direction variable
      case 'D':
      Serial.print("MOO: Adjust dir, now: ");
      dir = (dir+1)%4;
      Serial.println(dir);
      break;

      //Continue going forward until intersection
      case 'C':
      Serial.println("MOO: Continuing forward.");
      //do something
      break;

      //Drop
      case 'O':
      Serial.println("MOO: Dropping.");
      //call drop function
      dropBall();
      break;

      //Pick up
      case 'P':
      Serial.println("MOO: Picking up.");
      //call pickup function
      grabballnow();
      break;

      //Next instruction
      case 'N':
      Serial.print("MOO: Next instruction. #");
      instructionnumber = instructionnumber+1;
      Serial.println(instructionnumber);
      break;

      //Previous instruction (Back one)
      case 'B':
      Serial.print("MOO: Prev. instruction. #");
      if(instructionnumber>0)
        instructionnumber = instructionnumber-1;
      Serial.println(instructionnumber);
      break;

/*
      //a function very close to making the cut
      case 'M':
      Serial.println("MOO: Moo.");
      break;
 */     
      default:
      Serial.print("Function ");
      Serial.print(rcv);
      Serial.println(" not implemented.");
    }
  }
  debugging = false;
}

void serialCheck()
{
  if(Serial.available() && !debugging)
    serialEvent();
}

byte IRreceive()
{
  Serial.println("Waiting for IR.");
  IRrx.attach(IRPIN,-1);
  byte rcv = 0;
  while (rcv < 1)
  {
     rcv = IRrx.receive(200);
    switch(rcv)
    {
      case 0:
      case -1:
      break;
      case -2:
      break;
      case '1':
      Serial.println("Choosing path 1");
      break;
      case '2':
      Serial.println("Choosing path 2");
      break;
      case '3':
      Serial.println("Choosing path 3");
      break;
      default:
      Serial.print("Other IR get: ");
      Serial.println(rcv);
      rcv = -1;
    }
  }
  return rcv;
}
void getBall()
{
  //PICKUP BALL

  //drive to wall code
  //might need to use black line following code

 // unsigned long pickUpTime1 = millis();
  
  digitalWrite(LEFTDIR, HIGH);
  digitalWrite(RIGHTDIR, HIGH);
  analogWrite(LEFTSPD, 100);   //PWM Speed Control
  analogWrite(RIGHTSPD, 100);   //PWM Speed Control

while(dist <= 500)
  {
    dist = analogRead(IRFRONT);
    Serial.println(dist);
  }
  //AT WALL
  grabballnow();
}

void grabballnow()// grabbing part only
{
  TILT.write(90);
    analogWrite(LEFTSPD, 0);   //PWM Speed Control
    analogWrite(RIGHTSPD, 0);   //PWM Speed Control
    delay(1000);
   
//GRAB BALL
 

      force = 0;
      int n = 40;
      while (force < 600 && n < 175 ) {
        
        force = analogRead(GRIPSENSOR); //input pin used for gripper sensitivity (this should read HIGH or LOW
        Serial.print("F:");
        Serial.println(force);
        GRIP.write(n);
        n = n + 2; 
        
      delay(150);
    }
        delay(500);
        TILT.write(160);
        delay(500);
}
void dropBall()
{
  TILT.write(90);
  delay(500);
  GRIP.write(40);
  delay(500);
}



void getballWallCheck(){
//This will go under the line following code

   int dist = 0;
   dist = analogRead(IRFRONT);
   Serial.println(dist);
    if (dist<threshold1) {
    digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,255);
  analogWrite(LEFTSPD,255);
    }
 if (dist>threshold1 && dist<threshold2) {
    digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,120);
  analogWrite(LEFTSPD,120);
    }
    if (dist>threshold2) {
    digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,0);
  analogWrite(LEFTSPD,0);
    }
delay(10);
}


//***************PATH FINDING
//drive straight
void drStraight(){ //cfact removed (was used once drStraight(9)...?)
    if (analogRead(L) >= LTHRESH){        //if left sensor sees black
         analogWrite(E1, velRW); //go more left (slow down right a bit)
         analogWrite(E2, 0);
    }else if (analogRead(R) >= RTHRESH){  //if right sensor sees black
         analogWrite(E1, 0);        
         analogWrite(E2, velLW);
    }
    else if (analogRead(C) >= CTHRESH){
        analogWrite(E1, velRW);
        analogWrite(E2, velLW);
    }
    //go more left (slow down right a bit)
    //if right sensor sees black 
    //go more right (slow down left )
}
void drive(bool dir) {
//    velRW = 100 *velFact;
//    velLW = 100 *velFact;
    digitalWrite(M1, dir);
    digitalWrite(M2, dir);
    analogWrite(E1, velRW);
    analogWrite(E2, velLW);
}

void updateLoc(){
      if (lval >= LTHRESH && cval >= CTHRESH && rval >= RTHRESH){
          //passed an intersection
          delay(210);
          if (currentLoc.dir == 0){
              currentLoc.y += 1;
          }else if (currentLoc.dir == 1){
              currentLoc.x += 1;
          }else if (currentLoc.dir == 2){
              currentLoc.y -= 1;
          }else {
             currentLoc.x -=1;         
          }
    }
}  

void stopD() {
    analogWrite(E1, 0);
    analogWrite(E2, 0);
}
void pivot(int targetDir){
    int turnC = 0;
    float angle;
    int eCount = 0;
    delay(100);
    stopD;
    
    if (currentLoc.dir - targetDir == -1 || currentLoc.dir - targetDir == 3){ // that means a CW turn is needed
        digitalWrite(M1, LOW); // right wheel  
        digitalWrite(M2, HIGH);  // left wheel
        analogWrite(E1, velRW);
        analogWrite(E2, velLW);
        currentLoc.dir = targetDir;
        angle = 35.0;
        while(1){
              if (digitalRead(EL) == HIGH){
                  eCount+= 1;
                  Serial.println(eCount);
                  delay(10); //Might help?
              } 
              if (eCount >= angle*velFact && analogRead(C) >= CTHRESH){
                  stopD();
                  digitalWrite(M1, HIGH); // right wheel  
                  digitalWrite(M2, HIGH);  // left wheel
                  Serial.println("reached black line/done pivot");
                  break;
              }
        }
    }else if (currentLoc.dir - targetDir == 1 || currentLoc.dir - targetDir == -3){ // that means a CCW turn is needed
        digitalWrite(M1, HIGH); // right wheel  
        digitalWrite(M2, LOW);  // left wheel
        analogWrite(E1, velRW);
        analogWrite(E2, velLW);
        currentLoc.dir = targetDir;
        angle = 35.0;
        while(1){
              if (digitalRead(EL) == HIGH){
                  eCount+= 1;
                  Serial.println(eCount);
                  delay(10); //Might help?
              } 
              if (eCount >= angle*velFact && analogRead(C) >= CTHRESH){
                  stopD();
                  digitalWrite(M1, HIGH); // right wheel  
                  digitalWrite(M2, HIGH);  // left wheel
                  Serial.println("reached black line/done pivot");
                  break;
              }
        }
    }else if (currentLoc.dir - targetDir == 2 || currentLoc.dir - targetDir == -2){// means do a 180
        digitalWrite(M1, LOW); // right wheel  
        digitalWrite(M2, HIGH);  // left wheel
        analogWrite(E1, velRW);
        analogWrite(E2, velLW);
        currentLoc.dir = targetDir;
        angle = 75.0;
        while(1){
              if (digitalRead(EL) == HIGH){
                  eCount+= 1.0;
                  Serial.println(eCount);
                  delay(10); //Might help?
              } 
              if (eCount >= angle*velFact && analogRead(R) >= RTHRESH){                 // if it read two passes stop
                  delay(100);
                  stopD();
                  digitalWrite(M1, HIGH); // right wheel  
                  digitalWrite(M2, HIGH);  // left wheel
                  Serial.println("reached black line/done pivot");
                  break;
              }
        }
    }
}


bool makeReady(specLoc targetL,specLoc currentL){
    if (targetL.x == currentL.x && targetL.y == currentL.y && targetL.dir == currentL.dir){
          return true;
    }else if (targetL.x == currentL.x && targetL.y == currentL.y && targetL.dir != currentL.dir){
         pivot(targetL.dir);
         return true;
    }
}


void driveTo(specLoc tLoc){ //go somewhere in a straight line
     int direct = tLoc.dir;
     if (tLoc.x != currentLoc.x && tLoc.y != currentLoc.y){  // its too compex for this funciton
          Serial.println("too complex for driveTo!");
          return; 
     }if (currentLoc.dir !=tLoc.dir){
          pivot(direct);
          Serial.print("pivoting atm " );
     }
     while (1){
            lval = analogRead(L);
            rval = analogRead(R);
            cval = analogRead(C);
//            Serial.print("left light sensor value is " );
//            Serial.println(lval);
//            
//            Serial.print("center light sensor value is " );
//            Serial.println(cval);
//            
//            Serial.print("right light sensor value is " );
//            Serial.println(rval);
//            
//            Serial.print("right wheel velocity is " );
//            Serial.println(velRW);
//            
//            Serial.print("left wheel velocity is " );
//            Serial.println(velLW);

//             Serial.print("Current x is ");
//             Serial.println(currentLoc.x);
//
//             Serial.print("Current y is ");
//             Serial.println(currentLoc.y);
//
//             Serial.print("Current dir is ");
//             Serial.println(currentLoc.dir);
//
//             Serial.print("number of intersections is");
//             Serial.println(countInter);
            drive(true); 
            if (currentLoc.y == 0 && currentLoc.dir == 1){
                lval = rval;
            }
            else if (currentLoc.y == 0 && currentLoc.dir == 3){
                lval = rval;
            }
            Serial.print(lval);
            Serial.print("     ");
            Serial.print(cval);
            Serial.print("     ");
            Serial.println(rval);
            Serial.print("     ");

/*updateLoc*/if (lval >= LTHRESH && cval >= CTHRESH && rval >= RTHRESH) {//passed an intersection
                delay(150);
                countInter +=1;
                if (currentLoc.dir == 0){      //if facing north
                    currentLoc.y += 1;
                }else if (currentLoc.dir == 1){//if facing east
                    currentLoc.x += 1;
                }else if (currentLoc.dir == 2){// if facing south
                    currentLoc.y -= 1;
                }else if (currentLoc.dir == 3){
                   currentLoc.x -=1; 
                }         
             }
             drStraight(); //removed cfact???
             if (tLoc.x == currentLoc.x && tLoc.y == currentLoc.y){
                  delay(150);
                  stopD();
                  Serial.println("arrived at target");
                  return; 
             }
             Serial.print("Current x and y is ");
             Serial.print(currentLoc.x);
             Serial.println(currentLoc.y);

             Serial.print("Current dir is ");
             Serial.println(currentLoc.dir);
             
    }
}
