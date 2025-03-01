#include "timer.h"
#include <xc.h>

// define LED variable for convenience
#define LED LATAbits.LATA0          // LD1 is connected to RA0

int main() {
    TRISAbits.TRISA0 = 0;           // Set RA0 as output
    tmr_setup_period(TIMER1, 200);  // Set Timer1 for 200 ms and start it

    while (1) {
        LED = !LED;                 // Toggle LED state
        tmr_wait_period(TIMER1);    // Wait until next 
    }
}