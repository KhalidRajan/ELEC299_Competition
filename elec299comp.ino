//ELEC 299 competitions
//for teams 10 11 12

//---------PUT CAR NUMBER HERE (Team 10 = 0, Team  11 = 1, Team  12 = 2)
int teamcar = 1;

//libraries to include

#include <Servo.h>
#include "QSerial.h"
#include "./globVars.h"
#include "./rFunctions.h"

specLoc pathA[24] = {{4,0,1},{1,0,3},{1,0,2}, //first ball... all ball retrievals from this positions should end in {1,0,2}
{1,2,0},{0,2,3},{0,4,0},{0,2,2},{1,2,1},{1,0,2}, // second ball
{0,0,3},{1,0,1},{1,0,2}, //third ball
{1,2,0},{0,2,3},{1,2,1},{1,0,2}, //fourth ball
{1,3,0},{0,3,3},{1,3,1},{1,0,2}, //fifth ball
{1,4,0},{0,4,3},{1,4,1},{1,0,2}}; //sixth ball

specLoc pathB[18] = {{2,4,0},{4,4,1},{2,4,3},{2,0,2},//first ball... all ball retrievals from this position should end in {2,0,2}
{2,3,0},{1,3,3},{1,4,0},{1,3,2},{2,3,1},{2,0,2}, //second ball
{2,3,0},{3,3,1},{3,4,0},{3,3,3},{2,3,3},{2,0,2}, //third ball
{2,4,0},{2,0,2}}; //fourth ball

specLoc pathC[22] = {{3,1,0},{0,1,3},{3,1,1},{3,0,2}, //first ball.. all ball retrievals from this position should end in {3,0,2}
{3,2,0},{4,2,1},{4,4,0},{4,2,2},{3,2,3},{3,0,2}, //second ball
{3,1,0},{4,1,1},{3,1,3},{3,0,2}, //third ball
{3,2,0},{4,2,1},{3,2,3},{3,0,2}, //fourth ball
{3,3,0},{4,3,1},{3,3,3},{3,0,2}}; //fifth ball

//start of setup code
void setup() {
 //pin mode settings
  pinMode(GRIPSENSOR, INPUT); //gripforcesensor
  pinMode(EL, INPUT);
  pinMode(ER, INPUT);
  pinMode(RIGHTSPD, OUTPUT);
  pinMode(LEFTSPD, OUTPUT);
  pinMode(RIGHTDIR, OUTPUT);
  pinMode(LEFTDIR, OUTPUT);
    
Serial.begin(115200);
delay(200);
Serial.println("Starting ELEC299 Teams 10-12 Competition Program.");
delay(200);
Serial.print("StartIR is: ");
Serial.println(startIR-48);
if(startIR == '0')
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
  
  delay(2000);

  currentLoc = {1,-1,0};

  if (currentLoc.x == 1){
    Serial.println("driving to 1 0 0");
      driveTo({1,0,0});
  }else if (currentLoc.x == 2){
      driveTo({2,0,0});
  }else {
      driveTo({3,0,0});
  }
  Serial.println("Arrived at 1 0 0");
  driveTo({4,0,1});
      driveTo({4,2,0});
      driveTo({4,0,2});
      driveTo({1,0,3});
      driveTo({1,0,0});  
  /*
  int pathlength[] = {24,18,22};
  for(int a = 0;a<pathlength[pathselect];a++)
  {
    Serial.print("Drive sequence: Destination ");
    Serial.println(a);
    if(pathselect == 0)
    {
      driveTo(pathA[a]);
    }
    if(pathselect == 1)
    {
      driveTo(pathB[a]);
    }
    if(pathselect == 2)
    {
      driveTo(pathC[a]);
    }
    
  }
  */
  
  Serial.println("Finished path area.");

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
  while (rcv < 1 || rcv > 250)
  {
     rcv = IRrx.receive(200);
    switch(rcv)
    {
      case 0:
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


void collisionsense()
{
    // just does one check
   int dist = 0;
   dist = analogRead(IRFRONT);
   Serial.print("IR Prox value: ");
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

