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

//other constant definitions
#define threshold1 225
#define threshold2 350

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

//end of global variables

void setup() {
  
Serial.begin(115200);
pinMode(L1,OUTPUT);
pinMode(L2,OUTPUT);
pinMode(L3,OUTPUT);
//IRreceive(); // receive IR signal

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

void IRreceive()
{
  IRrx.attach(IRPIN,-1);
  char rcv = 0;
  while (rcv < 1)
  {
     rcv = IRrx.receive(200);
    switch(rcv)
    {
      case 0:
      case -1:
      Serial.println("IR Error 1");
      case -2:
      Serial.println("IR Error 2");
      break;
      default:
      Serial.print("IR get: ");
      Serial.println(rcv);
    }
  }
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

