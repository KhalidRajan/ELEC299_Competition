//ELEC 299 competitions
//for teams 10 11 12

//---------PUT CAR NUMBER HERE (Team 10 = 0, Team  11 = 1, Team  12 = 2)
//used for thresholds and other values that are unique to each car.
int teamcar = 1;
//IMPORTANT! UPDATED TO 'A' for AUTO!!!
byte startIR = 'A'; //'A' for check sensor for byte, '0', '1', '2' to force without checking.

//libraries to include

#include <Servo.h>
#include "QSerial.h"
#include "./globVars.h"
#include "./rFunctions.h"

//paths

specLoc pathA[20] = {{4,0,1,true},{1,0,3,false},{1,0,2,true}, //first ball... all ball retrievals from this positions should end in {1,0,2}
{1,2,0,false},{0,2,3,false},{0,4,0,true},{0,2,2,false},{1,2,1,false},{1,0,2,true}, // second ball
{0,0,3,true},{1,0,1,false},{1,0,2,true}, //third ball
{1,2,0,false},{0,2,3,true},{1,2,1,false},{1,0,2,true}, //fourth ball
{1,3,0,false},{0,3,3,true},{1,3,1,false},{1,0,2,true}}; //fifth ball};

specLoc pathB[22] = {{2,4,0,false},{4,4,1,true},{2,4,3,false},{2,0,2,true},//first ball... all ball retrievals from this position should end in {2,0,2}
{2,3,0,false},{1,3,3,false},{1,4,0,true},{1,3,2,false},{2,3,1,false},{2,0,2,true}, //second ball
{2,3,0,false},{3,3,1,false},{3,4,0,true},{3,3,2,false},{2,3,3,false},{2,0,2,true}, //third ball
{2,4,0,true},{2,0,2,true}, //fourth ball
{2,4,0,false},{0,4,3,true},{2,4,1,false},{2,0,2,true}}; //Fifth Ball

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
  pinMode(SPECIALPIN, OUTPUT);
    
Serial.begin(115200);
delay(200);
Serial.println("Starting ELEC299 Teams 10-12 Competition Program.");
delay(200);
Serial.print("StartIR is: ");
Serial.println(startIR-48);
Serial.print("Car number: Team 1");
Serial.println(teamcar);
if(startIR == 'A')
  startIR = IRreceive();
//check for received or forced startIR value
switch(startIR)
{
  case '0':
  pathselect = 0;
  break;
  case '1':
  pathselect = 1;
  break;
  case '2':
  pathselect = 2;
  break;
  default:
  Serial.println("Error. IR received wrong or startIR was not '0', '1', '2', '3'");
  pathselect = -1;
}


//motor initialization code area
  PAN.attach(PANPIN);    //pan fully left at 0, fully right at 180, 90 is center
  TILT.attach(TILTPIN);    //tilt horizontal at 70 degrees, verticle at 160, dont go below 15 degrees
  GRIP.attach(GRIPPIN);   //grip open entirely at 40 degrees to closed entirely at 180 degrees
  int panSet[] = {88,90,96};
  PAN.write(panSet[teamcar]);
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
  
  int pathlength[] = {20,22,22};
  for(instructionnumber = 0;instructionnumber<pathlength[pathselect];instructionnumber++)
  {
    Serial.print("Drive sequence: Destination ");
    Serial.println(instructionnumber);
    if(pathselect == 0)
    {
      driveTo(pathA[instructionnumber]);
    }
    if(pathselect == 1)
    {
      driveTo(pathB[instructionnumber]);
    }
    if(pathselect == 2)
    {
      driveTo(pathC[instructionnumber]);
    }
    
  }
  
  
  Serial.println("Finished path area.");

//delay before starting loop.
delay(1000);
}






void loop() {
  serialCheck();
  stopD();
  delay(3000);
  Serial.println("Execution Finished.");
  if(!Serial.available())
    songOfTheCentury();
}



void serialEvent(){
  debugging = true;
  while(Serial.available()){
    delay(1000);
    char rcv = (char)Serial.read();
    switch (rcv){
      case '0':
      Serial.println("MOO: Resetting to start position.");
      stopD();
      Serial.println("Please wait...");
      delay(1500);
      asm volatile ("  jmp 0");//soft reset function
      break;
      
      //Right turn
      case 'R':
      Serial.println("MOO: Turning Right");
      pivot((currentLoc.dir+1)%4);
      break; //have a kitkat

      //Left turn
      case 'L':
      Serial.println("MOO: Turning Left");
      pivot((currentLoc.dir+3)%4);
      break;

      //Start or stop toggle
      case 'S':
            running = !running;
      if(!running)
        {
          Serial.println("MOO: Stopping");
          stopD();
          while(!running)
          {
            delay(1000);
            serialEvent();
          }
        }
        
      else
        {
          Serial.println("MOO: Starting");
        }

      break;

      //Increment X position variable
      case 'X':
      Serial.print("MOO: Adjust x, now: ");
      currentLoc.x = (currentLoc.x+1)%5;
      Serial.println(currentLoc.x);
      break;

      //Increment Y position variable
      case 'Y':
      Serial.print("MOO: Adjust y, now: ");
      currentLoc.y = (currentLoc.y+1)%6;
      Serial.println(currentLoc.y);
      break;

      //Increment direction variable
      case 'D':
      Serial.print("MOO: Adjust dir, now: ");
      currentLoc.dir = (currentLoc.dir+1)%4;
      Serial.println(currentLoc.dir);
      break;

      //Continue going forward until intersection
      case 'C':
      Serial.println("MOO: Continuing forward.");
      //do something
      velRW[teamcar] = 100.0*velFact[teamcar];
      velLW[teamcar] = 98.0*velFact[teamcar];
      digitalWrite(RIGHTDIR, HIGH);
      digitalWrite(LEFTDIR, HIGH);
      analogWrite(RIGHTSPD, velRW[teamcar]);
      analogWrite(LEFTSPD, velLW[teamcar]);
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

      case 'V':
      Serial.print("MOO: Reverse");
      drive(false);
      Serial.println(instructionnumber);
      break;
      
      case 'H':
      Serial.print("MOO: Halting Movement.");
      stopD();
      break;


      //a function very close to making the cut
      case 'M':
      Serial.println("MOO: Moo.");
      songOfTheCentury();
      break;
           
      default:
      Serial.print("Function ");
      Serial.print(rcv);
      Serial.println(" not implemented.");
    }
  }
  if(running)
    Serial.println("Exit Debug Mode. Will try to resume normal operation.");
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
    serialCheck();
     rcv = IRrx.receive(200);
    switch(rcv)
    {
      case 0:
      break;
      case 255:
      break;
      case '0':
      Serial.println("Choosing path 1");
      break;
      case '1':
      Serial.println("Choosing path 2");
      break;
      case '2':
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
  int tiltangle[] = {30,70,50};
  
  TILT.write(tiltangle[teamcar]);
    stopD();  //PWM Speed Control
    delay(1000);
   
//GRAB BALL
 

      int force = 0;
      int n = 40;
      int attempts = 1;
      while (force < 600 && n < 180) {
        serialCheck();
        force = analogRead(GRIPSENSOR); //input pin used for gripper sensitivity (this should read HIGH or LOW
        if(force>0)
        {
          Serial.print("F:");
          Serial.println(force);
          serialCheck();
        }
        GRIP.write(n);
        n = n + 2; 
          if(n > 179 && force < 600){
            attempts = attempts +1;
            Serial.print("Missed object! ");
            
            if(attempts>5)
            {
              Serial.println("Too many attempts. Giving up.");
              grabfail = true;
               break;
            }
            Serial.print(attempts);
            Serial.print("th attempt of 5.");
           TILT.write(160);
            GRIP.write(40);
            n = 40;
            //re approach and try again
            digitalWrite(RIGHTDIR,LOW);
          digitalWrite(LEFTDIR,LOW);
           analogWrite(RIGHTSPD,velRW[teamcar]);
          analogWrite(LEFTSPD,velLW[teamcar]);
          int revstep[] = {5,5,5};
          encdrive(revstep[teamcar]);
          digitalWrite(RIGHTDIR,HIGH);
          digitalWrite(LEFTDIR,HIGH);
          Serial.println("Waiting for bumper again");
        while(digitalRead(LBUMP)==HIGH && digitalRead(RBUMP)== HIGH)
        {
          serialCheck();
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

          digitalWrite(RIGHTDIR,LOW);
          digitalWrite(LEFTDIR,LOW);
           analogWrite(RIGHTSPD,velRW[teamcar]);
          analogWrite(LEFTSPD,velLW[teamcar]);
          int revstep2[] = {3,3,3};
          encdrive(revstep2[teamcar]);
          TILT.write(tiltangle[teamcar]);
    stopD();  //PWM Speed Control
    delay(1000);

          
            continue;
         }
      delay(70);
    }
    n = n + 20;
    GRIP.write(n);
      holding = true;
        delay(500);
        TILT.write(160);
        delay(500);
}
void dropBall()
{
  int tiltangle[] = {27,70,50};
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
  serialCheck();
  int eCount = 0;
          int inter = digitalRead(EL);
          while(eCount < gostep){ 
                    if (inter != digitalRead(EL)){
                      inter = digitalRead(EL);
                      eCount++;
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
          serialCheck();
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
           analogWrite(RIGHTSPD,velRW[teamcar]);
          analogWrite(LEFTSPD,velLW[teamcar]);
          int revstep[] = {3,3,3};
          encdrive(revstep[teamcar]);
        //backed up car
         grabballnow();
        }
        delay(300);
          
        
        
        pivot((currentLoc.dir + 2)%4);

  digitalWrite(RIGHTDIR,HIGH);
  digitalWrite(LEFTDIR,HIGH);
  analogWrite(RIGHTSPD,velRW[teamcar]);
  analogWrite(LEFTSPD,velLW[teamcar]);
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
            if(analogRead(GRIPSENSOR) > 600)
            {//SUCCESSFUL GRAB!
              holding = true;
              grabfail = false;
            }
            else
            {//FAILED TO GRAB...
              holding = false;
            }
            stopD();
            digitalWrite(RIGHTDIR,HIGH);
          digitalWrite(LEFTDIR,HIGH);
           analogWrite(RIGHTSPD,velRW[teamcar]);
          analogWrite(LEFTSPD,velLW[teamcar]);
          int fwstep[] = {2,8,6};
          encdrive(fwstep[teamcar]);
            break;
          }
        
        }
}

void songOfTheCentury()
{
      for(int a = 0;a<100;a++)
      {
        digitalWrite(SPECIALPIN,HIGH);
        delayMicroseconds(6000-20*a);
        digitalWrite(SPECIALPIN,LOW);
        delayMicroseconds(6000-20*a);
      }
}

