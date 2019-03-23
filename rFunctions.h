//Robot Functions

#include "Arduino.h"
#include <Servo.h>
#include <math.h>
#include "QSerial.h"
/* 
    0 N
    1 E
    2 S
    3 W
*/
//    0
//  3   1
//    2   
typedef struct specLoc{
  int x;
  int y;
  int dir;
//  boolean isTar;
} specLoc ;       

// variables for sensors



int countInter;
int i;
int LTHRESH[] = {850,990,850}; //black line sensor threshold for each team's  car
int RTHRESH[] = {850,990,850}; //black
int CTHRESH[] = {850,990,950}; //black
int PROXTHRESH[] = {420,420,420};
int INTERSECTSTEP[]={6,8,6};

float velFact = 0.8;
float velRW = 100.0*velFact;
float velLW = 98.0*velFact;

Servo pan, tilt, grab;

//Functions
void pivot(int targetDir);
void drive(bool dir);
void driveTo(specLoc tLoc);
void stopD();
void adjSpeed(float rF, float lF);

specLoc currentLoc;
specLoc diceLoc[15] = {{0,0,3},{0,1,3},{0,2,3},{0,3,3},{0,4,3},
                       {0,4,0},{1,4,0},{2,4,0},{3,4,0},{4,4,0},
                       {4,0,1},{4,1,1},{4,2,1},{4,3,1},{4,4,1}};
specLoc homeLocS[3] = {{1,0,2},
                      {2,0,2},
                      {3,0,2}};
specLoc homeLocN[3] = {{1,0,0},
                      {2,0,0},
                      {3,0,0}};
                      
void stopD() {
    analogWrite(RIGHTSPD, 0);
    analogWrite(LEFTSPD, 0);
}
void drive(bool dir) {
    digitalWrite(RIGHTDIR, dir);
    digitalWrite(LEFTDIR, dir);
    analogWrite(RIGHTSPD, velRW);
    analogWrite(LEFTSPD, velLW);
}
void adjSpeed(float rF, float lF){
    analogWrite(RIGHTSPD, velLW*rF);
    analogWrite(LEFTSPD, velRW*lF);
    return;
}					  
void driveTo(specLoc tLoc){ //go somewhere in a straight line
   int lval;
  int rval;
  int cval;
   boolean flag;
   int dist,r;
	 int intL,intR;
     if (tLoc.x != currentLoc.x && tLoc.y != currentLoc.y){  // its too compex for this funciton
          Serial.println("too complex for driveTo!");
          return; 
     }if (currentLoc.dir !=tLoc.dir){
          pivot(tLoc.dir);
          Serial.print("pivoting atm " );
     }
     drive(true); 
     while (1){
            lval = analogRead(L);
            rval = analogRead(R);
            cval = analogRead(C);
      			intL = analogRead(L);
      			intR = analogRead(R);
            if (currentLoc.dir == 1){
                intR = intL;
            }
            else if (currentLoc.dir == 3){
                intL = intR;
            }
/*updateLoc*/if (intL >= LTHRESH[teamcar] && cval >= CTHRESH[teamcar] && intR >= RTHRESH[teamcar]){//passed an intersection
                countInter += 1;
                if (currentLoc.dir == 0){  currentLoc.y += 1;}      //if facing north
                else if (currentLoc.dir == 1){currentLoc.x += 1;} //if facing east
                else if (currentLoc.dir == 2){currentLoc.y -= 1;} // if facing south
                else if (currentLoc.dir == 3){currentLoc.x -= 1;}
                int inter = digitalRead(EL);
                int eCount = 0;
                adjSpeed(1,1);
        				while(eCount < INTERSECTSTEP[teamcar]){ 
          					if (inter != digitalRead(EL)){
          					  inter = digitalRead(EL);
          					  eCount++;
          					  Serial.print("i");
          					  Serial.println(eCount);
          					}
        				}
            }
            if (tLoc.x == currentLoc.x && tLoc.y == currentLoc.y){
                Serial.println("arrived at target");
                break; 
            }
            if (lval >= LTHRESH[teamcar]){   //if left sensor sees black
               adjSpeed(1, 0);
            }else if (rval >= RTHRESH[teamcar]){  //if right sensor sees black
               adjSpeed(0, 1);
            }else if (cval >= CTHRESH[teamcar]){
               adjSpeed(1, 1);
            }        
      			r = analogRead(IRr);
      			while (r >= PROXTHRESH[teamcar]){
      			   adjSpeed(velLW*0.5, velRW*0.5);
               r = analogRead(IRr);
               if (r < PROXTHRESH[teamcar]){
                   adjSpeed(1,1);
                   break;
               }
      			}
     }
     stopD();
     return;
}

void pivot(int targetDir){
    int turnC = 0;
    float angle;
    int eCount = 0;
    boolean flag;
    stopD();  
  	if(currentLoc.dir == targetDir){
  		  return;
  	}
    if (currentLoc.dir - targetDir == -1 || currentLoc.dir - targetDir == 3){ // that means a CW turn is needed
        digitalWrite(RIGHTDIR, LOW); // right wheel  
        digitalWrite(LEFTDIR, HIGH);  // left wheel
        angle = 5.0;
    }
    else if (currentLoc.dir - targetDir == 1 || currentLoc.dir - targetDir == -3){ // that means a CCW turn is needed
        digitalWrite(RIGHTDIR, HIGH); // right wheel  
        digitalWrite(LEFTDIR, LOW);  // left wheel
        angle = 5.0;
    }
    else if (currentLoc.dir - targetDir == 2 || currentLoc.dir - targetDir == -2){// means do a 180
        digitalWrite(RIGHTDIR, LOW); // right wheel  
        digitalWrite(LEFTDIR, HIGH);  // left wheel
        angle = 14.0;
    }
	  adjSpeed(1,1);
    while(1){
              if (digitalRead(EL) == HIGH && flag){
                  eCount+= 1;
                  Serial.println(eCount);
                  flag = false;
              } 
              else if (digitalRead(EL) == LOW && !flag){
                  flag = true;
                  eCount+= 1;
              }
              delayMicroseconds(1000);
              if (eCount >= angle*velFact && analogRead(C) >= CTHRESH[teamcar]){
                  currentLoc.dir = targetDir;
                  Serial.println("reached black line/done pivot");
                  break;
              }
    }
    stopD();
    return;
}
