/*
 * File:   main2.c
 * Author: smuss
 *
 * Created on 06 March 2025, 17:11
 */


#include "xc.h"
#include "timer.h"


#define LED2 LATGbits.LATG9

void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    IEC1bits.INT1IE = 0;    // disable int1
    IFS0bits.INT1IF = 0; // reset interrupt flag
    LED2 = !LED2;
    IEC1bits.INT1IE = 1;    // reenable int1
}

int main(void) {
    
    // Disable analog mode 
    ANSELEbits.ANSE8 = 0;
    // Set pin E8 as input (button)
    TRISEbits.TRISE8 = 1;
    
    TRISGbits.TRISG9 = 0;   //set as out
    LED2 = 0;               // reset
    
    RPINR0bits.INT1R = 0x58;    // Associate RE8 to INT1 functionality
    INTCON2bits.GIE = 1;        // Enable global interrupt
    IFS1bits.INT1IF = 0;        // Clear flag
    IEC1bits.INT1IE = 1;        // Enable specific interrupt
    
    while (1) {
        
    }
    
    return 0;
}
