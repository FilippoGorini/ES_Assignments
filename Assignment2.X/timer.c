#include "timer.h"
#include <xc.h>

#define FOSC 144000000  // 144 MHz system clock (Fcy = Fosc/2)
#define FCY (FOSC / 2)  // 72 MHz instruction cycle
#define PRESCALER 256
#define TICKS_PER_MS (FCY / PRESCALER / 1000)  // 281.25 ticks per ms

void tmr_setup_period(int timer, int ms) {
    unsigned int timer_count = (281.25 * ms);  // Convert ms to timer ticks

    if (timer == TIMER1) {
        T1CONbits.TON = 0;    // Stop Timer1
        T1CONbits.TCKPS = 3;  // Prescaler 1:256 (TCKPS: 00=1, 01=8, 10=64, 11=256)
        PR1 = timer_count;    // Set timer period
        TMR1 = 0;             // Reset Timer1 counter
        IFS0bits.T1IF = 0;    // Clear Timer1 interrupt flag
        T1CONbits.TON = 1;    // Start Timer1
    }
}

int tmr_wait_period(int timer) {
    int expired = 0;

    if (timer == TIMER1) {
        if (IFS0bits.T1IF) {
            expired = 1;  // Expired before function call
        }
        while (!IFS0bits.T1IF);  // Wait for Timer1 flag
        IFS0bits.T1IF = 0;       // Clear flag
    }

    return expired;
}
