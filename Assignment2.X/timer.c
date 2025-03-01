#include "timer.h"
#include <xc.h>

// The Fosc in our setup is 144 MHz. Therefore, the instruction cycle frequency
// is Fcy = Fosc / 2 = 72 Mhz. In our application we will always use the timer 
// in the first of the 4 functioning modes (see slide 15), which uses the 
// peripheral clock Fp (which is the same as Fcy at 72 MHz). At 72 MHz, 200 ms
// corresponds to 72000000 / 5 = 14400000 ticks. Therefore we need to set the 
// prescaler to 256 in order for PR1 to be representable with 16 bits:
// 14400000/256 = 56250 < 65535, OK

void tmr_setup_period(int timer, int ms) {
    // The timer ticks in 1 ms can be calculated as:
    // Fcy / 256 / 1000 = 281.25
    unsigned int timer_count = (281.25 * ms);   // Convert ms to timer ticks

    if (timer == TIMER1) {
        T1CONbits.TON = 0;          // Stop Timer1
        IFS0bits.T1IF = 0;          // Clear Timer1 interrupt flag
        TMR1 = 0;                   // Reset Timer1 counter
        PR1 = timer_count;          // Set timer period
        T1CONbits.TCKPS = 3;        // Prescaler 1:256 (TCKPS: 00=1, 01=8, 10=64, 11=256)
        T1CONbits.TON = 1;          // Start Timer1
    }
}

int tmr_wait_period(int timer) {
    int expired = 0;

    if (timer == TIMER1) {
        if (IFS0bits.T1IF) {
            expired = 1;            // Expired before function call
        }
        while (!IFS0bits.T1IF);     // Wait for Timer1 flag
        IFS0bits.T1IF = 0;          // Clear flag
    }

    return expired;
}


void tmr_wait_ms(int timer, int ms){   
    if (ms <= 200){
       tmr_setup_period(timer, ms);
       tmr_wait_period(timer);
       T1CONbits.TON = 0;
    }
}
