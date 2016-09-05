/* 
 * File:   NSdecoder_main.c
 * Author: T_Maki
 *
 * Created on September 4, 2016, 8:40 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "uart.h"


//init config start
#pragma config CLKOUTEN = OFF
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config BOREN = OFF
#pragma config BORV = HI
#pragma config PWRTE = ON
#pragma config WDTE = OFF 
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config LVP = OFF
#pragma config STVREN = OFF
#pragma config PLLEN = OFF 
#pragma config FOSC = INTOSC
#pragma config VCAPEN = OFF

//for delay routine
#define _XTAL_FREQ 16000000

/*
 *msec delay
 */
void arg_delay_ms(unsigned int x) {
    while(x) {
        __delay_ms(1);
        x--;
    }
}

/*
 * main
 */
void main (void) {
    //clock speed = 16MHz
    OSCCON = 0b01111010;
    
    
    char work;
 
    PORTA = 0x00;           // PORTAを初期化
    PORTB = 0x00;           // PORTBを初期化
    PORTC = 0x00;           // PORTCを初期化
    ANSELA = 0b00000000 ;    // AN0~4:digital
    ANSELB = 0b00000000 ;    // AN12-AN13:digital
    TRISA = 0b00000000;     // PORTAの入出力設定
    TRISB = 0b00000000;     // PORTBの入出力設定
    TRISC = 0b10000000;     // RC7を入力に割当てる
 
    
    initUART();             // 非同期式シリアル通信設定
    
    char buff[6];
    char ch;
    unsigned char count = 0;
    buff[0] = 0;
    
    //main loop
    while (1) {
        // Add your application code
        //5byte受信
        ch = getch();
        while (1) {
            buff[count] = ch;
            count++;
            buff[count] = 0;
            if (count == 5){                
                break;
            }
            ch = getch();
        }

        //正常受信なら、点灯状況変更
        if (buff[0] == 'N' && buff[1] == 'C' && (buff[4] & 0x0f) == 0x05) {
            PORTA = buff[2];
            PORTB = buff[3];
            RC0 = (buff[4]>>7)&1;
            RC1 = (buff[4]>>6)&1;
            RC2 = (buff[4]>>5)&1;
            RC3 = (buff[4]>>4)&1;
        }
        
        
        count = 0;
        buff[0] = 0;
        __delay_ms(1);
    }

    
}

