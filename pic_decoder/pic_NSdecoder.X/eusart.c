/**
  EUSART Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart.c

  @Summary
    This is the generated driver implementation file for the EUSART driver using MPLAB® Code Configurator

  @Description
    This header file provides implementations for driver APIs for EUSART.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC16F1938
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.34
        MPLAB             :  MPLAB X v2.35 or v3.00
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */

/**
  Section: Included Files
 */
#include "eusart.h"

/**
  Section: EUSART APIs
 */

void EUSART_Initialize(void) {
    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP async_noninverted_sync_fallingedge; RCIDL idle; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUDCON = 0x48;

    // SPEN enabled; OERR no_error; RX9 8-bit; RX9D 0x0; CREN enabled; ADDEN disabled; SREN disabled; FERR no_error; 
    RCSTA = 0x90;

    // TRMT TSR_empty; TX9 8-bit; TX9D 0x0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave_mode; 
    TXSTA = 0x26;

    // Baud Rate = 9600; SPBRGL 207; 
    SPBRGL = 0xCF;

    // Baud Rate = 9600; SPBRGH 0; 
    SPBRGH = 0x00;

}

uint8_t EUSART_Read(void) {

    while (!PIR1bits.RCIF) {
    }

    if (1 == RCSTAbits.OERR) {
        // EUSART error - restart

        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    return RCREG;
}

void EUSART_Write(uint8_t txData) {
    while (0 == PIR1bits.TXIF) {
    }

    TXREG = txData; // Write the data byte to the USART.
}

char getch(void) {
    return EUSART_Read();
}

void putch(char txData) {
    EUSART_Write(txData);
}
/**
  End of File
 */

