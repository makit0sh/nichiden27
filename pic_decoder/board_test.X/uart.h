/* 
 * File:   uart.h
 * Author: T_Maki
 *
 * Created on September 4, 2016, 8:45 PM
 */


#ifndef __UART_H__
#define __UART_H__
 
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 16000000 // 16MHz
#endif
 
#define BAUDRATE 9600       // 9.6kbps
#define TX9_RX9_BIT 0       // 0: 8bit, 1: 9bit
#define BRGH_BIT 1          // 0: 低速サンプル指定, 1: 高速サンプル指定
 
#if TX9_RX9_BIT == 1
#define TX9_RX9_DATA 0x40
#else
#define TX9_RX9_DATA 0x00
#endif
 
#if BRGH_BIT == 1
#define BRGH_DATA 0x04
#define SPBRG_DATA ((unsigned char)(((_XTAL_FREQ / 16) / BAUDRATE) - 1))
#else
#define BRGH_DATA 0x00
#define SPBRG_DATA ((unsigned char)(((_XTAL_FREQ / 64) / BAUDRATE) - 1))
#endif
 
// prototype declaration
void initUART();
void putch(unsigned char byte);
unsigned char getch();
unsigned char getche();
 
#endif  // __UART_H__
