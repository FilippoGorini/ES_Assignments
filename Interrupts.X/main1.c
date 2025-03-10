#include "xc.h"
#include "timer.h"

#define LED1 LATAbits.LATA0
#define LED2 LATGbits.LATG9


// When using T2+T3 as a single 32-bit timer, the interrupt flag is raised by T3
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;                  // Clear Timer3 Interrupt Flag
    LED2 = !LED2;                       // Toggle LED2
}


int main(void) {

    TRISAbits.TRISA0 = 0;               // Set RA0 as output
    TRISGbits.TRISG9 = 0;               // Set RG9 as output
    IEC0bits.T3IE = 1;                  // Enable Timer3 interrupt
    LED1 = 0;                           // Reset LED1
    LED2 = 0;                           // Reset LED2

    tmr_setup_period_32(TIMER2, 500);   // Start a 32-bit timer (T2+T3)
    tmr_setup_period(TIMER1, 200);      // Start a 16-bit timer (T1)

    while(1){
        LED2 = !LED2;                   // Toggle LED2
        tmr_wait_period(TIMER1);        // Wait until next period
    }

    return 0;
}