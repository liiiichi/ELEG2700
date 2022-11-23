/*
  ELEG2700 Reference program for Project Development Week 1 Task 1
  Board: Arduino Nano
  Version: 202210

  Output: Servo1 (D6)
  Output: Servo2 (D9)
  Output: 7-segment (D10,D11)
  Output: motors (D3,D2), (D5,D4)
  Input: IR sensor long range (A5)
  Input: IR sensor left (A4)
  Input: IR sensor middle (A3)
  Input: IR sensor right (A2)
*/
#include "ELEG2700.h"
#include <Servo.h>
#define BLK 0
#define WHT 1

// Use the values you tuned in Lab 6
#define RIGHT_POS 173
#define REAR_POS 85
#define LEFT_POS 7
#define CLAMP_OPEN 145
#define CLAMP_RELEASE 100
#define CLAMP_CLOSE 60
#define NORMAL_SPEED 250
#define TURN_SPEED 180
#define LEFT_OFFSET 0
#define RIGHT_OFFSET 30
  
FSMClass FSM1;                            // The given FSM is named 'FSM1'.
TM1637DisplayClass LEDDisplay(D10, D11);  // 7-segment LED Display is connected to D10,D11 for debug
ServoClass Servo1(D6);                    // Servo Motor 1: for Left/Right/Rear position of clamp
ServoClass Servo2(D9);                    // Servo Motor 2: for Open/Close/Release of clamp
SensorClass S1(A5);                       // Long-range IR sensor (for cup detection)
SensorClass S2(A4);                       // Short-range IR sensor
SensorClass S3(A3);                       // Short-range IR sensor
SensorClass S4(A2);                       // Short-range IR sensor

SensorClass S5(A0);
SensorClass S6(A1);
SensorClass S7(A7);

MotorClass MotorR(D3,D2);                 // Right Motor
MotorClass MotorL(D5,D4);                 // Left Motor

// Define total number of BBB joints in one cycle
#define NUM_JOINT 5

// Global variable to count the number of joints reached
int joint = 0;
// Global variable to indicate cup is detected or not
int cup_det = 0;
// Global variable to count the number of cups catch
int cup = 0;

//===== Basically, no need to modify setup() and loop() ====
void setup()
{
  Serial.begin(115200); 
  LEDDisplay.setBrightness(15);
  FSM1.init(S_999);             // must have this line, you can change the first state of the FSM
  Servo1.init();
  Servo2.init();
}

void loop()
{
    FSM1.run();     // Important, don't modify this line              
}
//================= Students add STATES below this line ====================
//----------start of state S_999 -----
void S_999()   
{
  if(FSM1.doTask())
  {
    if(cup < 5 or cup >= 12)
    Servo1.setValue(LEFT_POS);  //RIGHT_POS
    else if(cup >= 5)
    Servo1.setValue(RIGHT_POS);
    Servo2.setValue(CLAMP_OPEN);
    LEDDisplay.setValue(999);
  }
  if (FSM1.getTime() > 500) FSM1.transit(S_101);      
}
//------------------------------------
void S_101()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(100); 
  }
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_102);    // Car is at the center of the black track -> keeps going straight forward
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_103);    // Car is left-biased -> turn right
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_104);    // Car is right-biased -> turn left
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK) {
    joint = joint+1;
    FSM1.transit(S_201);    // Car reach the joint point
  }
}
//------------------------------------
void S_102()   
{
  if(FSM1.doTask())
  {
    // Car goes straight forward
    LEDDisplay.setValue(102); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);  
  }
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== WHT)) FSM1.transit(S_101);  
}
//------------------------------------
void S_103()    
{
  if(FSM1.doTask())
  {
    // Car turn right
    LEDDisplay.setValue(103); 
    // Add action statement here!   
    MotorR.setSpeed(TURN_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);  
  }
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== BLK)) FSM1.transit(S_101);   
}
//------------------------------------
void S_104()   
{
  if(FSM1.doTask())
  {
    // Car turn left
    LEDDisplay.setValue(104); 
    // Add action statement here!
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(TURN_SPEED-LEFT_OFFSET);  
  }
  if (!(S2.getHiLow()== BLK && S4.getHiLow()== WHT)) FSM1.transit(S_101);   
}
//------------------------------------
void S_201()   
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second only
    LEDDisplay.setValue((joint*1000)+201); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);      
  }
  if(cup < 9){
      if (FSM1.getTime() >100) FSM1.transit(S_202); 
  }
  else if(cup >= 9){
    if (FSM1.getTime() >100) FSM1.transit(S_203); 
  }
}
//------------------------------------
void S_202()   
{
  if(FSM1.doTask())
  {
    // Car turns left
    LEDDisplay.setValue((joint*1000)+202); 
    // Add action statement here!
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(0);  
  }
  if (S2.getHiLow()== WHT) FSM1.transit(S_301); 
}
//------------------------------------
void S_203()   
{
  if(FSM1.doTask())
  {
    // Car turns left
    LEDDisplay.setValue((joint*1000)+203); 
    // Add action statement here!
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);  
  }
  if (S2.getHiLow()== WHT) FSM1.transit(S_301); 
}
//------------------------------------
void S_301()   
{
  if(FSM1.doTask())
  {
    // Follow the black track after turning left at the joint point
    LEDDisplay.setValue((joint*1000)+301); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);   
  }
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_302);
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_303);
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_304);
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_305);
  
  // Cup is Detected!
  if (S1.getHiLow()== BLK)  FSM1.transit(S_401);
}
//------------------------------------
void S_302()   
{
  if(FSM1.doTask())
  {
    // Car goes straight
    LEDDisplay.setValue(302); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);   
  }
  // Cup is Detected!
  if (S1.getHiLow()== BLK)  FSM1.transit(S_401);
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== WHT)) FSM1.transit(S_301); 
}
//------------------------------------
void S_303()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(303);
    MotorR.setSpeed(0);  
    MotorL.setSpeed(TURN_SPEED);
  }
  if (S1.getHiLow()== BLK)  FSM1.transit(S_401);
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== BLK)) FSM1.transit(S_301);
}
//------------------------------------
void S_304()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(304); 
    MotorR.setSpeed(TURN_SPEED); 
    MotorL.setSpeed(0);  
  }
  if (S1.getHiLow()== BLK)  FSM1.transit(S_401);
  if (!(S2.getHiLow()== BLK && S4.getHiLow()== WHT)) FSM1.transit(S_301);
}
//------------------------------------
void S_305()   
{
  // Car reach the Joint Point
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second only
    joint = joint+1;
    LEDDisplay.setValue((joint*1000)+305); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);      
  }
  if (S1.getHiLow()== BLK)  FSM1.transit(S_401);
  if ((FSM1.getTime() > 300) && (joint < NUM_JOINT)) FSM1.transit(S_301);
}
//------------------------------------
void S_401()
{
  if(FSM1.doTask())
  {
    // Stop the car and catch the cup
    LEDDisplay.setValue((joint*1000)+401);
    MotorR.setSpeed(0); 
    MotorL.setSpeed(0);  
    Servo2.setValue(CLAMP_CLOSE);
    cup = cup +1;
    cup_det = 1;
  }
  if (FSM1.getTime() > 500) FSM1.transit(S_402);
}
//------------------------------------
void S_402()   
{
  if(FSM1.doTask())
  {
    // Move the clamp to the back
    LEDDisplay.setValue((joint*1000)+402); 
    MotorR.setSpeed(0); 
    MotorL.setSpeed(0);  
    Servo1.setValue(REAR_POS);
  }
  if(cup < 9)
  if (FSM1.getTime() > 100) FSM1.transit(S_501); 
  else if(cup >= 9)
  if (FSM1.getTime() > 100) FSM1.transit(S_901);
}
//------------------------------------
void S_501()   
{
  if(FSM1.doTask())
  {
    // Follow the black track
    LEDDisplay.setValue((joint*1000)+501);
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);
  }
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_502);
  if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_503);
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_504);
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_601);    // Car reach the joint point again
  // Cup is Detected!
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
}
//------------------------------------
void S_502()   
{
  if(FSM1.doTask())
  {
    // Car goes straight
    LEDDisplay.setValue(302); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);   
  }
  // Cup is Detected!
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== WHT)) FSM1.transit(S_501); 
}
//------------------------------------
void S_503()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(303);
    MotorR.setSpeed(0);  
    MotorL.setSpeed(TURN_SPEED);
  }
    if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== BLK)) FSM1.transit(S_501);
}
//------------------------------------
void S_504()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(304); 
    MotorR.setSpeed(TURN_SPEED); 
    MotorL.setSpeed(0);  
  }
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S2.getHiLow()== BLK && S4.getHiLow()== WHT)) FSM1.transit(S_501);
}
//------------------------------------
void S_601()   
{
  // Car reach the Joint Point
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second only
    joint = joint+1;
    LEDDisplay.setValue((joint*1000)+601); 
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);      
  }

  // ---------------------------------------------------------------------//
  // ** The time value 100 is subject to be changed for different cars ** //
  // ** You can modify it to 300 or 500 to have your best performance  ** //
  // ---------------------------------------------------------------------//
  // If it is the joint point apart from start zone, keep going
  if ((FSM1.getTime() > 300) && (joint < NUM_JOINT)) FSM1.transit(S_501);
  // If it is at the 1st joint point again (i.e., end zone), turn left
  if ((FSM1.getTime() > 100) && (joint >= NUM_JOINT)) FSM1.transit(S_602);
}
//------------------------------------
void S_602()   
{
  if(FSM1.doTask())
  {
    // Car turns left
    LEDDisplay.setValue((joint*1000)+602); 
    // Add action statement here! -- Car turns left after the joint point
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET); 
    MotorL.setSpeed(0);
  }
  if (S2.getHiLow()==WHT && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_701); 
}
//------------------------------------
void S_701()   
{
  if(FSM1.doTask())
  {
    // Follow the black track after turning left at the joint point
    LEDDisplay.setValue((joint*1000)+701);
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);
  }

  // Add a transition here: If car reaches the end zone
  if (S2.getHiLow()== WHT && S3.getHiLow()== WHT && S4.getHiLow()== WHT) FSM1.transit(S_800);
}
//-----------------------------------
void S_800()
{
  if(FSM1.doTask())
  {
    // Stop the car
    LEDDisplay.setValue((joint*1000)+801); 
    MotorR.setSpeed(NORMAL_SPEED);
    MotorL.setSpeed(-NORMAL_SPEED);
  }
if (S2.getHiLow()== WHT && S3.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_801);
}
//------------------------------------
void S_801()   
{
  if(FSM1.doTask())
  {
    // Stop the car
    LEDDisplay.setValue((joint*1000)+801); 
    MotorR.setSpeed(0);
    MotorL.setSpeed(0);   
  }
  if (FSM1.getTime() > 300) FSM1.transit(S_802);
}
//------------------------------------
void S_802()
{
  if(FSM1.doTask())
  {
    //release the cup
    LEDDisplay.setValue((joint*1000)+802); 
    Servo2.setValue(CLAMP_RELEASE);
  }
    if (FSM1.getTime() > 100) {
      FSM1.transit(S_999);
      joint = 0;
      cup_det = 0;
    }
}
//------------------------------------
void S_901()   
{
  if(FSM1.doTask())
  {
    // Follow the black track
    LEDDisplay.setValue((joint*1000)+901);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));
  }
  if (S5.getHiLow()== WHT && S6.getHiLow()== BLK && S7.getHiLow()== WHT) FSM1.transit(S_902);
  if (S5.getHiLow()== WHT && S6.getHiLow()== BLK && S7.getHiLow()== BLK) FSM1.transit(S_903);
  if (S5.getHiLow()== BLK && S6.getHiLow()== BLK && S7.getHiLow()== WHT) FSM1.transit(S_904);
  if (S5.getHiLow()== WHT && S6.getHiLow()== WHT && S7.getHiLow()== WHT) FSM1.transit(S_801);    // Car reach the joint point again
  // Cup is Detected!
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
}
//------------------------------------
void S_902()   
{
  if(FSM1.doTask())
  {
    // Car goes straight
    LEDDisplay.setValue(902); 
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));   
  }
  // Cup is Detected!
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S5.getHiLow()== WHT && S7.getHiLow()== WHT)) FSM1.transit(S_901); 
}
//------------------------------------
void S_903()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(903);
    MotorR.setSpeed(0);  
    MotorL.setSpeed(-TURN_SPEED);
  }
    if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S5.getHiLow()== WHT && S7.getHiLow()== BLK)) FSM1.transit(S_901);
}
//------------------------------------
void S_904()   
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue(904); 
    MotorR.setSpeed(-TURN_SPEED); 
    MotorL.setSpeed(0);  
  }
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S5.getHiLow()== BLK && S7.getHiLow()== WHT)) FSM1.transit(S_901);
}
