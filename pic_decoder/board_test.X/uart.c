#include <xc.h>
#include <stdio.h>
 
#include "uart.h"
 
void initUART()
{
    
    SPBRG = SPBRG_DATA;
    TXSTA = (TX9_RX9_DATA | BRGH_DATA | 0x20);
    RCSTA = (TX9_RX9_DATA | 0x90);
}
 
void putch(unsigned char byte)
{
    while(!TXIF){
    }
    TXREG = byte;
}
 
unsigned char getch()
{
    while(!RCIF){
    }
 
    return RCREG;
}
 
unsigned char getche()
{
    unsigned char c;
 
    c = getch();
    putch(c);
 
    return c;
}