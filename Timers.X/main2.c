#include "timer.h"
#include <xc.h>

// Define LED variable for convenience
#define LED LATAbits.LATA0          // LD1 is connected to RA0

int main() {
    TRISAbits.TRISA0 = 0;         // Set RA0 as output
    
    while (1) {
        LED = 0;
        tmr_wait_ms(TIMER1, 200);
        LED = 1;
        tmr_wait_ms(TIMER1, 200);  
    }
}
