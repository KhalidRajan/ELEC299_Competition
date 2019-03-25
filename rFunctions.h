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
  boolean action;
} specLoc ;       

// variables for sensors




const int LTHRESH[] PROGMEM = {970,990,850}; //black line sensor threshold for each team's  car
const int PROXTHRESH[] PROGMEM = {320,320,320};
const int INTERSECTSTEP[] PROGMEM ={6,8,6};

const float velFact[] PROGMEM = {1.4,1.2,1.1};
float velRW[] = {108.0*velFact[teamcar],98.0*velFact[teamcar],100.0*velFact[teamcar]};
float velLW[] = {92.0*velFact[teamcar],100.0*velFact[teamcar],98.0*velFact[teamcar]};

Servo pan, tilt, grab;

//Functions
void pivot(int targetDir);
void drive(bool dir);
void driveTo(specLoc tLoc);
void stopD();
void adjSpeed(float rF, float lF);
void getSequence();
void serialCheck();
void songOfTheCentury();
specLoc currentLoc;
                      
void stopD() {
    analogWrite(RIGHTSPD, 0);
    analogWrite(LEFTSPD, 0);
}
void drive(bool dir) {
    digitalWrite(RIGHTDIR, dir);
    digitalWrite(LEFTDIR, dir);
    analogWrite(RIGHTSPD, velRW[teamcar]);
    analogWrite(LEFTSPD, velLW[teamcar]);
}
void adjSpeed(float rF, float lF){
    analogWrite(RIGHTSPD, velLW[teamcar]*rF);
    analogWrite(LEFTSPD, velRW[teamcar]*lF);
    return;
}					  
void driveTo(specLoc tLoc){ //go somewhere in a straight line
   int lval;
  int rval;
  int cval;
   boolean flag;
   int dist,r;
	 int intL,intR;
  int countInter;
int i;
  Serial.print("Driving to (");
  Serial.print(tLoc.x);
  Serial.print(",");
  Serial.print(tLoc.y);
  Serial.println(")");
     if (tLoc.x != currentLoc.x && tLoc.y != currentLoc.y){  // its too compex for this funciton
          Serial.println("Too Complex!");
          return; 
     }if (currentLoc.dir !=tLoc.dir){
          pivot(tLoc.dir);
          Serial.print("Pivoting!" );
     }
     drive(true); 
     while (1){
            serialCheck();
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
/*updateLoc*/if (intL >= LTHRESH[teamcar] && cval >= LTHRESH[teamcar] && intR >= LTHRESH[teamcar]){//passed an intersection
                countInter += 1;
                if (currentLoc.dir == 0){  currentLoc.y += 1;}      //if facing north
                else if (currentLoc.dir == 1){currentLoc.x += 1;} //if facing east
                else if (currentLoc.dir == 2){currentLoc.y -= 1;} // if facing south
                else if (currentLoc.dir == 3){currentLoc.x -= 1;}
                Serial.print("CX: ");
                Serial.print(currentLoc.x);
                Serial.print("CY: ");
                Serial.print(currentLoc.y);
                int inter = digitalRead(EL);
                int eCount = 0;
                adjSpeed(1,1);
        				while(eCount < INTERSECTSTEP[teamcar]){ 
          					if (inter != digitalRead(EL)){
          					  inter = digitalRead(EL);
          					  eCount++;
                     serialCheck();
                     if(forcebreak)
                      {
                        Serial.println("FQ: Inter");
                        forcebreak = false;
                        break;
                      }
          					}
        				}
            }
            if (tLoc.x == currentLoc.x && tLoc.y == currentLoc.y){
                Serial.println("At tLoc!");
                if(tLoc.action)//needs to pick up/drop at target.
                  {
                    Serial.println("Getting!");
                    getSequence();
                  }
                break; 
            }
            if (lval >= LTHRESH[teamcar]){   //if left sensor sees black
              Serial.print('x');
               adjSpeed(1, 0);
            }else if (rval >= LTHRESH[teamcar]){  //if right sensor sees black
               adjSpeed(0, 1);
               Serial.print('v');
            }else if (cval >= LTHRESH[teamcar]){
               adjSpeed(1, 1);
            }        
      			r = analogRead(IRr);
            serialCheck();
      			while (r >= PROXTHRESH[teamcar]){
            serialCheck();
            if(forcebreak)
            {
              Serial.println("will crash!");
              forcebreak = false;
            }
      			   adjSpeed(0.5, 0.5);
               r = analogRead(IRr);
               if (r < PROXTHRESH[teamcar]){
                   adjSpeed(1,1);
                   break;
               }
      			}
     }
     serialCheck();
     stopD();
     return;
}

void pivot(int targetDir){
    int turnC = 0;
    const float rightangleturn[] = {5,5,5};
    const float uturn[] = {14,14,14};
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
        angle = rightangleturn[teamcar];
    }
    else if (currentLoc.dir - targetDir == 1 || currentLoc.dir - targetDir == -3){ // that means a CCW turn is needed
        digitalWrite(RIGHTDIR, HIGH); // right wheel  
        digitalWrite(LEFTDIR, LOW);  // left wheel
        angle = rightangleturn[teamcar];
    }
    else if (currentLoc.dir - targetDir == 2 || currentLoc.dir - targetDir == -2){// means do a 180
        digitalWrite(RIGHTDIR, LOW); // right wheel  
        digitalWrite(LEFTDIR, HIGH);  // left wheel
        angle = uturn[teamcar];
    }
	  adjSpeed(1,1);
    while(1){
      serialCheck();
              if (digitalRead(EL) == HIGH && flag){
                  eCount+= 1;
                  flag = false;
              } 
              else if (digitalRead(EL) == LOW && !flag){
                  flag = true;
                  eCount+= 1;
              }
              delayMicroseconds(200);
              if (eCount >= angle*velFact[teamcar] && analogRead(C) >= LTHRESH[teamcar]){
                  currentLoc.dir = targetDir;
                  Serial.println("BL detect/Good Pivot");
                  break;
              }
    }
    stopD();
    return;
}
