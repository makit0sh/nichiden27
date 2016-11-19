
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
uint8_t LightPattern[2][4][120] = { // put user setting pattern here
  {/* leave here blank for Auto mode! */},
  {
    {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,127,126,124,122,120,118,116,114,
      111,109,107,105,103,101,99,97,95,93,90,88,86,84,82,80,78,76,74,72,69,67,65,63,61,59,57,55,53,50,48,46,44,42,40,38,
      36,34,32,29,27,25,23,21,19,17,15,13,11,8,6,4,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,
      170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250,255,250,245,240,235,231,226,221,216,211,206,
      201,196,192,187,182,177,172,167,162,157,153,148,143,138,133,128,123,118,113,109,104,99,94,89,84,79,74,70,65,60,
      55,50,45,40,35,31,26,21,16,11,6,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,11,17,22,28,34,39,45,51,56,62,68,73,79,85,90,96,102,107,113,
      119,124,130,136,142,147,153,159,164,170,176,181,187,193,198,204,210,215,221,227,232,238,244,249,255,247,240,232,
      225,217,209,202,194,187,179,171,164,156,149,141,133,126,118,111,103,95,88,80,73,65,57,50,42,35,27,19,12,4,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,
      12,18,24,30,36,42,48,54,60,66,72,77,83,89,95,101,107,113,119,125,131,137,143,149,155,161,167,173,179,185,191,185,
      179,173,167,161,155,149,143,137,131,125,119,113,107,101,95,89,83,77,72,66,60,54,48,42,36,30,24,18,12,6,0}
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
  mode_flag = (mode_flag + 1) % 2; // mode_flag = (0, 1) < LightPattern
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
    unsigned int input = analogRead(VR2);
    unsigned int pos = round(120 / 1023.0 * input);
    Serial.println(input);
    REP(i,4){
      SoftPWMSet(VolumeToPin[i][direction + 1], LightPattern[mode_flag][i][pos]);
      SoftPWMSet(VolumeToPin[i][!direction + 1], 0);
      delayMicroseconds(100);
    }
  }
}

