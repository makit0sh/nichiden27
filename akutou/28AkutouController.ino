#include <SoftPWM.h>
#define REP(i,n) for(i=0;i<(n);i++)

void setup()
{
  SoftPWMBegin();
  
  SoftPWMSet(5, 0);
  SoftPWMSet(6, 0);
  SoftPWMSet(7, 0);
  SoftPWMSet(8, 0);

  SoftPWMSetFadeTime(5, 1000, 1000);
  SoftPWMSetFadeTime(6, 1000, 1000);
  SoftPWMSetFadeTime(7, 1000, 1000);
  SoftPWMSetFadeTime(8, 1000, 1000);
}

void loop()
{
  delay(1000);
  SoftPWMSet(5, 0);
  SoftPWMSet(6, 0);
  SoftPWMSet(7, 0);
  SoftPWMSet(8, 0);
  delay(1000);
}
