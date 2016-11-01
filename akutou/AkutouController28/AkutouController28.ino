
/*****************************************************************************************
*******************************   28AkutouController.ino   *******************************
* Created: 2016/11/01 22:53:42
* Author: K_Ito(Nichiden_27)
*
* Powered by bhagman's SoftPWM Library(https://github.com/bhagman/SoftPWM/wiki).
*****************************************************************************************/

#include <SoftPWM.h>

/***** Arduino Nano pin setting *****/
/* Interrupt pin */
#define INT_SW 2
/* East-West switch */
#define DIR_SW 19
/* PWM pins */
#define PWM_W1 5
#define PWM_W2 6
#define PWM_W3 7
#define PWM_W4 8
#define PWM_E1 9
#define PWM_E2 10
#define PWM_E3 11
#define PWM_E4 12
/* for 7 segment driver 74HC4511 */
#define SEG0 18
#define SEG1 4
#define SEG2 3
#define SEG3 17
/* Analog pins */
#define VR1 0 // A0
#define VR2 1 // A1
#define VR3 2 // A2
#define VR4 6 // A6(analog only)
#define VR5 7 // A7(analog only)
/************************************/
#define REP(i,n) for(i=0;i<(n);i++)

int sensor_value = 0;
volatile int mode_flag = 0;

void setup(){
  Serial.begin(9600);
  pinMode(INT_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_SW), modeChange, FALLING);
  pinMode(DIR_SW, INPUT_PULLUP);

  SoftPWMBegin();

  SoftPWMSet(PWM_W1, 0);
}

void loop(){
  for(;;){}
}

void modeChange(){
  mode_flag++;
  delayMicroseconds(100);
}

void manualMode(){
  sensor_value = analogRead(VR1);
  SoftPWMSet(PWM_W1, map(sensor_value, 0, 1023, 0, 255));
}

void autoMode(){
  return;
}

