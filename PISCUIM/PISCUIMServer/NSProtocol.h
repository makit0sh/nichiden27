#ifndef NSProtocol_h
#define NSProtocol_h

#include "arduino.h"
#include <String.h>

class NSProtocol
{
    private:
        String constellationName[20];
        char pinStatus[20] = {}; // 1:set, 0:clear

    public:
        NSProtocol();
        
        void setConstellationName(String p01, String p02,String p03,String p04,String p05,String p06,String p07,String p08,String p09,String p10,String p11,String p12,String p13,String p14,String p15,String p16,String p17,String p18,String p19,String p20);

        boolean setPort(int pinNo);
        boolean setPort(String name);
        boolean clearPort(int pinNo);
        boolean clearPort(String name);

        void allSet();
        void allClear();
        
        char* getPacket(char* c);

        String getJsonStatus();
};

#endif
        
