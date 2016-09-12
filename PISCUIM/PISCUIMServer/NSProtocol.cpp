#include "NSProtocol.h"
#include "arduino.h"
#include <String.h>

NSProtocol::NSProtocol()
{
  constellationName[0] = "P01";
  constellationName[1] = "P02";
  constellationName[2] = "P03";
  constellationName[3] = "P04";
  constellationName[4] = "P05";
  constellationName[5] = "P06";
  constellationName[6] = "P07";
  constellationName[7] = "P08";
  constellationName[8] = "P09";
  constellationName[9] = "P10";
  constellationName[10] = "P11";
  constellationName[11] = "P12";
  constellationName[12] = "P13";
  constellationName[13] = "P14";
  constellationName[14] = "P15";
  constellationName[15] = "P16";
  constellationName[16] = "P17";
  constellationName[17] = "P18";
  constellationName[18] = "P19";
  constellationName[19] = "P20";
}

boolean NSProtocol::setConstellationName(int pinNo, String s)
{
  if ( 1 <= pinNo && pinNo <= 20) {
    constellationName[pinNo - 1] = s;
    return true;
  }
  return false;
}

boolean NSProtocol::getConstellationName(int pinNo, String& s)
{
  if ( 1 <= pinNo && pinNo <= 20) {
    s = constellationName[pinNo - 1];
    return true;
  }
  return false;
}

boolean NSProtocol::setPort(int pinNo)
{
  if ( 1 <= pinNo && pinNo <= 20) {
    pinStatus[pinNo - 1] = 1;
  } else {
    return false;
  }
  return true;
}

boolean NSProtocol::setPort(String name)
{
  int i;
  for (i = 0; i < 20; i++) {
    if (name.equals(constellationName[i])) {
      pinStatus[i] = 1;
      return true;
    }
  }
  return false;
}

boolean NSProtocol::clearPort(int pinNo)
{
  if ( 1 <= pinNo && pinNo <= 20) {
    pinStatus[pinNo - 1] = 0;
  } else {
    return false;
  }
  return true;
}

boolean NSProtocol::clearPort(String name)
{
  int i;
  for (i = 0; i < 20; i++) {
    if (name.equals(constellationName[i])) {
      pinStatus[i] = 0;
      return true;
    }
  }
  return false;
}

void NSProtocol::allSet()
{
  int i;
  for (i = 0; i < 20; i++) {
    pinStatus[i] = 1;
  }
}

void NSProtocol::allClear()
{
  int i;
  for (i = 0; i < 20; i++) {
    pinStatus[i] = 0;
  }
}

char* NSProtocol::getPacket(char* c)
{
  c[0] = 'N';
  c[1] = 'S';
  c[2] = pinStatus[0] + pinStatus[1] * 2 + pinStatus[2] * 4 + pinStatus[3] * 8 + pinStatus[4] * 16 + pinStatus[5] * 32 + pinStatus[6] * 64 + pinStatus[7] * 128;
  c[3] = pinStatus[8] + pinStatus[9] * 2 + pinStatus[10] * 4 + pinStatus[11] * 8 + pinStatus[12] * 16 + pinStatus[13] * 32 + pinStatus[14] * 64 + pinStatus[15] * 128;
  c[4] = pinStatus[16] * 128 + pinStatus[17] * 64 + pinStatus[18] * 32 + pinStatus[19] * 16 + 5;
  return c;
}

String NSProtocol::getJsonStatus()
{
  String s;
  int i;
  s = "{";
  for ( i = 0; i < 19; i++) {
    s += "\"";
    s += constellationName[i];
    s += "\":";
    s += pinStatus[i];
    s += ",";
  }
  s += "\"";
  s += constellationName[i];
  s += "\":";
  s += pinStatus[i];
  s += "}";
  return s;
}
