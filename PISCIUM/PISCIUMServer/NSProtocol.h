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
        
        boolean setConstellationName(int pinNo, String s);
        boolean getConstellationName(int pinNo, String& s);
        
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
        
