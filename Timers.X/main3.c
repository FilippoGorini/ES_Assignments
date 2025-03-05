#include "timer.h"
#include <xc.h>

// Define LED variables for convenience
#define LED1 LATAbits.LATA0             // LD1 is connected to RA0
#define LED2 LATGbits.LATG9             // LD2 is connected to RG9

int main() {
    int ret = 0;
    TRISAbits.TRISA0 = 0;               // Set RA0 as output
    TRISGbits.TRISG9 = 0;               // Set RG9 as output
    
    tmr_setup_period(TIMER1, 200);
    int delay = 50; // 200, 2000
    while (1) {
        tmr_wait_ms(TIMER2, delay);
        LED1 = !LED1;                   // Toggle LED1 state
        ret = tmr_wait_period(TIMER1);
        if (ret) {
            LED2 = 1;
        } 
        else {
            LED2 = 0;
        }
    }
}
