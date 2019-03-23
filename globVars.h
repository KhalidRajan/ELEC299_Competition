//pin definitions
#define IRPIN A0

#define IRFRONT A4
#define GRIPSENSOR A5
#define RIGHTSPD 6
#define RIGHTDIR 7
#define LEFTSPD 5
#define LEFTDIR 4
#define MAXPULSE 65000
#define RESOLUTION 20
#define EL 12
#define ER 13
#define L A3
#define R A2
#define C A1
#define IRr A4
#define LBUMP 3
#define RBUMP 2
#define PANPIN 10
#define TILTPIN 9
#define GRIPPIN 8

//other constant definitions
#define threshold1 225
#define threshold2 350
//pathfinding constants



//typedefs and structs 

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

int pathselect;

//end of global variables


//VARIABLES IMPORTED FROM OTHER PROJECTS,WHEN POSSIBLE MERGE WITH OTHER VARIABLES
//path tracking variables


//IR variables
byte startIR = '1'; //'0' for check sensor for byte, '1', '2', '3' to force without checking.

//grab drop ball
boolean holding = false;