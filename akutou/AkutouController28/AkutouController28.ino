
/*****************************************************************************************
*******************************   AkutouController28.ino   *******************************
* Created: 2016/11/01 22:53:42
* Author: K_Ito(Nichiden_27)
*
* Supposed to run on Arduino Nano.
* Powered by bhagman's SoftPWM Library(https://github.com/bhagman/SoftPWM/wiki).
*****************************************************************************************/

#include <SoftPWM.h>
#define REP(i,n) for(int i=0;i<(n);i++)
#define SIZE_OF(x) sizeof(x) / sizeof(x[0])

/***** Arduino Nano pin setting *****/
/* Interrupt pin */
#define INT_SW 2
/* East-West switch */
#define DIR_SW 19 // LOW for West, HIGH for East
/* for 7 segment driver 74HC4511 */
#define SEG0 18
#define SEG1 4
#define SEG2 3
#define SEG3 17
/* PWM pins */
#define PWM_W1 5
#define PWM_W2 6
#define PWM_W3 7
#define PWM_W4 8
#define PWM_E1 9
#define PWM_E2 10
#define PWM_E3 11
#define PWM_E4 12
/* Analog pins */
#define VR1 0 // A0(master volume)
#define VR2 1 // A1
#define VR3 2 // A2
#define VR4 6 // A6(analog only)
#define VR5 7 // A7(analog only)
/************************************/

const int PWM_Pins[8] = {PWM_W1, PWM_W2, PWM_W3, PWM_W4, PWM_E1, PWM_E2, PWM_E3, PWM_E4};
const int VolumeToPin[4][3] = {
  {VR2, PWM_W1, PWM_E1},
  {VR3, PWM_W2, PWM_E2},
  {VR4, PWM_W3, PWM_E3},
  {VR5, PWM_W4, PWM_E4},
};
const int FadeTime = 100; // Time in milliseconds of 0 -> 255 or 255 -> 0
int LightPattern[2][4][60] = { // put user setting pattern here
  {/* leave here blank for Auto mode! */},
  {
    {255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,26,38,51,64,77,90,102,115,128,115,102,90,77,64,51,38,26,13,0,0,0,0,0,0,0,0,0,0,0},
    {0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,26,38,51,64,77,90,102,115,128,141,154,166,179,192,205,218,230,243,255,243,230,218,205,192,179,166,154,141,128,115,102,90,77,64,51,38,26,13,0},
    {0,0,255,0,0,26,51,77,102,128,154,179,205,230,255,230,205,179,154,128,102,77,51,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,255,0,13,26,38,51,64,77,90,102,115,128,125,122,119,116,114,111,108,105,102,99,96,93,90,87,84,82,79,76,73,70,67,64,61,58,55,53,50,47,44,41,38,35,32,29,27,24,21,18,15,12,9,6,3,0,0}
  }
}; // light controll pattern(max 255)
volatile int mode_flag = 0; // save current mode number
volatile unsigned long time_pre = 0, time_cur; // avoid chattering

struct HC4511{
  int pin[4];
  HC4511(int p0, int p1, int p2, int p3): pin{p0,p1,p2,p3}{
    REP(i,4){
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], HIGH); // all HIGH for blank
    }
  }
  void write(int value){
      REP(i,4){
        digitalWrite(pin[i], value % 2);
        value /= 2;
      }
  }
  HC4511& operator = (int value){write(value);return *this;}
};

HC4511 segLED(SEG0, SEG1, SEG2, SEG3);

void modeSelect(){
  time_cur = millis();
  Serial.print("time_cur - time_pre: ");
  Serial.println(time_cur - time_pre);
  if(time_cur - time_pre <= 200) return;
  mode_flag = (mode_flag + 1) % 10; // mode_flag = (0, 1, .. , 10) < LightPattern
  Serial.print("mode ");
  Serial.println(mode_flag);
  time_pre = time_cur;
}

void setup(){
  Serial.begin(9600);
  Serial.println("setup begin");
  pinMode(INT_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_SW), modeSelect, FALLING);
  pinMode(DIR_SW, INPUT_PULLUP);
  segLED = mode_flag;
  /* Initialize SoftPWM library */
  SoftPWMBegin();
  REP(i,SIZE_OF(PWM_Pins)){
    SoftPWMSet(PWM_Pins[i], 0);
    SoftPWMSetFadeTime(PWM_Pins[i], FadeTime, FadeTime);
  }
  Serial.println("setup end");
}

void loop(){
  pinMode(INT_SW, INPUT_PULLUP);
  segLED = mode_flag;
  int direction = digitalRead(DIR_SW); // West: 0, East: 1
  if(!mode_flag){ // Manual mode
    REP(i,4){
      int input = analogRead(VolumeToPin[i][0]);
      SoftPWMSet(VolumeToPin[i][direction + 1], map(input, 0, 1023, 0, 255));
      SoftPWMSet(VolumeToPin[i][!direction + 1], 0);
      delayMicroseconds(100);
    }
  }
  else{ // Auto mode
    int input = analogRead(VR1);
    int position = round(SIZE_OF(LightPattern[mode_flag][0]) * input / 1023);
    REP(i,4){
      SoftPWMSet(VolumeToPin[i][direction], LightPattern[mode_flag][i][position]);
      delayMicroseconds(100);
    }
  }
}

