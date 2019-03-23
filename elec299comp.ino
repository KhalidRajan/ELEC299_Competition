//ELEC 299 competitions
//for teams 10 11 12

//---------PUT CAR NUMBER HERE (Team 10 = 0, Team  11 = 1, Team  12 = 2)
//used for thresholds and other values that are unique to each car.
int teamcar = 0;
byte startIR = '1'; //'0' for check sensor for byte, '1', '2', '3' to force without checking.

//libraries to include

#include <Servo.h>
#include "QSerial.h"
#include "./globVars.h"
#include "./rFunctions.h"

specLoc pathA[24] = {{4,0,1,true},{1,0,3,false},{1,0,2,true}, //first ball... all ball retrievals from this positions should end in {1,0,2}
{1,2,0,false},{0,2,3,false},{0,4,0,true},{0,2,2,false},{1,2,1,false},{1,0,2,true}, // second ball
{0,0,3,true},{1,0,1,false},{1,0,2,true}, //third ball
{1,2,0,false},{0,2,3,true},{1,2,1,false},{1,0,2,true}, //fourth ball
{1,3,0,false},{0,3,3,true},{1,3,1,false},{1,0,2,true}, //fifth ball
{1,4,0,false},{0,4,3,true},{1,4,1,false},{1,0,2,true}}; //sixth ball

specLoc pathB[18] = {{2,4,0,false},{4,4,1,true},{2,4,3,false},{2,0,2,true},//first ball... all ball retrievals from this position should end in {2,0,2}
{2,3,0,false},{1,3,3,false},{1,4,0,true},{1,3,2,false},{2,3,1,false},{2,0,2,true}, //second ball
{2,3,0,false},{3,3,1,false},{3,4,0,true},{3,3,2,false},{2,3,3,false},{2,0,2,true}, //third ball
{2,4,0,true},{2,0,2,true}}; //fourth ball

specLoc pathC[22] = {{3,1,0,false},{0,1,3,true},{3,1,1,false},{3,0,2,true}, //first ball.. all ball retrievals from this position should end in {3,0,2}
{3,2,0,false},{4,2,1,false},{4,4,0,true},{4,2,2,false},{3,2,3,false},{3,0,2,true}, //second ball
{3,1,0,false},{4,1,1,true},{3,1,3,false},{3,0,2,true}, //third ball
{3,2,0,false},{4,2,1,true},{3,2,3,false},{3,0,2,true}, //fourth ball
{3,3,0,false},{4,3,1,true},{3,3,3,false},{3,0,2,true}}; //fifth ball

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
Serial.print("Car number: Team 1");
Serial.println(teamcar);
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
  PAN.attach(PANPIN);    //pan fully left at 0, fullly right at 180, 90 is center
  TILT.attach(TILTPIN);    //tilt horizontal at 70 degrees, verticle at 160, dont go below 15 degrees
  GRIP.attach(GRIPPIN);   //grip open entirely at 40 degrees to closed entirely at 180 degrees
  PAN.write(90);
  delay(100);
  TILT.write(160);
  delay(100);
  GRIP.write(40);
  delay(100);


  currentLoc = {pathselect+1,-1,0,false};

  if (currentLoc.x == 1){
    Serial.println("driving to 1 0 0");
      driveTo({1,0,0,false});
  }else if (currentLoc.x == 2){
      driveTo({2,0,0,false});
  }else {
      driveTo({3,0,0,false});
  }
  Serial.println("Arrived at starting loc");
  
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
  
  
  Serial.println("Finished path area.");

//delay before starting loop.
delay(1000);
}






void loop() {
  serialCheck();
  stopD();
  delay(1000);
  Serial.println("Execution Finished.");
  //test code for get and drop only
//  getBall();
//  delay(1000);
//  dropBall();
//  delay(1000);
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
int dist = 1;
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
  int tiltangle[] = {30,70,70};
  
  TILT.write(tiltangle[teamcar]);
    analogWrite(LEFTSPD, 0);   //PWM Speed Control
    analogWrite(RIGHTSPD, 0);   //PWM Speed Control
    delay(1000);
   
//GRAB BALL
 

      int force = 0;
      int n = 40;
      while (force < 600 && n < 180) {
        
        force = analogRead(GRIPSENSOR); //input pin used for gripper sensitivity (this should read HIGH or LOW
        Serial.print("F:");
        Serial.println(force);
        GRIP.write(n);
        n = n + 2; 
          if(n > 175 && force < 600){
            Serial.println("Missed object! Trying again");
           TILT.write(tiltangle[teamcar]);
            GRIP.write(40);
            n = 40;
            continue;
         }
      delay(70);
    }
    if(n<180)
      holding = true;
        delay(500);
        TILT.write(160);
        delay(500);
}
void dropBall()
{
  int tiltangle[] = {30,70,70};
  TILT.write(170);
  delay(100);
  TILT.write(tiltangle[teamcar]);
  GRIP.write(40);
  delay(500);
 int tiltangle2[] = {160,160,160};
  TILT.write(tiltangle2[teamcar]);
  holding = false;
}


void encdrive(int gostep)
{
  int eCount = 0;
          int revstep[] = {3,3,3};
          int inter = digitalRead(EL);
          while(eCount < gostep){ 
                    if (inter != digitalRead(EL)){
                      inter = digitalRead(EL);
                      eCount++;
                      Serial.print("ed");
                      Serial.println(eCount);
                    }
                }
}

void getSequence(){
//This will go under the line following code
    digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,120);
  analogWrite(LEFTSPD,120);
Serial.println("Waiting for bumper");
        while(digitalRead(LBUMP)==HIGH && digitalRead(RBUMP)== HIGH)
        {
          int lval = analogRead(L);
           int  rval = analogRead(R);
           int  cval = analogRead(C);
          if (lval >= LTHRESH[teamcar]){   //if left sensor sees black
               adjSpeed(1, 0);
            }else if (rval >= LTHRESH[teamcar]){  //if right sensor sees black
               adjSpeed(0, 1);
            }else if (cval >= LTHRESH[teamcar]){
               adjSpeed(1, 1);
            }        
        }
        Serial.println("Will now grab/drop.");
        if (holding)
        {
          dropBall();
        }

        //may require adjustment later
        else
        {
          digitalWrite(RIGHTDIR,LOW);
          digitalWrite(LEFTDIR,LOW);
           analogWrite(RIGHTSPD,velRW);
          analogWrite(LEFTSPD,velLW);
          int revstep[] = {3,3,3};
          encdrive(revstep[teamcar]);
        //backed up car
         grabballnow();
        }
        delay(300);
        
        
        
        pivot((currentLoc.dir + 2)%4);

  digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,velRW);
  analogWrite(LEFTSPD,velLW);
while(1)
        {
        int lval = analogRead(L);
           int  rval = analogRead(R);
           int  cval = analogRead(C);
          if (lval >= LTHRESH[teamcar]){   //if left sensor sees black
               adjSpeed(1, 0);
            }else if (rval >= LTHRESH[teamcar]){  //if right sensor sees black
               adjSpeed(0, 1);
            }else if (cval >= LTHRESH[teamcar]){
               adjSpeed(1, 1);
            }        

        if (lval >= LTHRESH[teamcar] && cval >= LTHRESH[teamcar] && rval >= LTHRESH[teamcar])
          {
            Serial.println("Finished get/drop");
            stopD();
            digitalWrite(RIGHTDIR,HIGH);
          digitalWrite(LEFTDIR,HIGH);
           analogWrite(RIGHTSPD,velRW);
          analogWrite(LEFTSPD,velLW);
          encdrive(INTERSECTSTEP[teamcar]);
            break;
          }
        
        }
}
