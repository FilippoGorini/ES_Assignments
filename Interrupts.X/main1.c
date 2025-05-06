#include <xc.h>
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/general_purpose_lib.h"


// When using T2+T3 as a single 32-bit timer, the interrupt flag is raised by T3
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;                  // Clear Timer3 Interrupt Flag
    LED2 = !LED2;                       // Toggle LED2
}


int main(void) {

    leds_init();
    
    IEC0bits.T3IE = 1;                  // Enable Timer3 interrupt

    tmr32_setup_period(TIMER2, 500);    // Start a 32-bit timer (T2+T3)
    tmr_setup_period(TIMER1, 200);      // Start a 16-bit timer (T1)

    while(1){
        LED1 = !LED1;                   // Toggle LED1
        tmr_wait_period(TIMER1);        // Wait until next period
    }

    return 0;
}