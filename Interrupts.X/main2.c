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
    IFS1bits.INT1IF = 0;    // reset interrupt flag
    LED2 = !LED2; 
}

int main(void) {
    
    // Disable analog mode 
    ANSELEbits.ANSE8 = 0;
    // Set pin E8 as input (button)
    TRISEbits.TRISE8 = 1;
    
    TRISGbits.TRISG9 = 0;       // Set as out
    LED2 = 0;                   // Reset
    
    RPINR0bits.INT1R = 0x58;    // Associate INT1 functionality to RE8 (RP88=0x58)
    INTCON2bits.GIE = 1;        // Enable global interrupt
    IFS1bits.INT1IF = 0;        // Clear flag
    IEC1bits.INT1IE = 1;        // Enable specific interrupt
    
    tmr_setup_period(TIMER1, 200);
    while(1){
        LED2 = !LED2;
        tmr_wait_period(TIMER1);
    }
    
    return 0;
}
