#include "xc.h"
#include "timer.h"

#define LED2 LATGbits.LATG9


void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    IFS1bits.INT1IF = 0;        // Reset INT1 interrupt flag
    LED2 = !LED2;               // Toggle LED2
}


int main(void) {

    ANSELEbits.ANSE8 = 0;       // Disable analog mode
    TRISEbits.TRISE8 = 1;       // Set pin E8 as input (button)
    TRISGbits.TRISG9 = 0;       // Set RG9 as output
    LED2 = 0;                   // Reset LED2

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