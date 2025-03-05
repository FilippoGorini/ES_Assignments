#include "timer.h"
#include <xc.h>

#define FOSC 144000000  // 144 MHz Oscillator
#define FCY (FOSC / 2)  // Instruction cycle frequency (72 MHz)
#define MAX_DELAY 233   // max valid int value for ms to not overflow PR1 [ms]
// Max chunk size (233.2 ms) is achieved with a prescaler of 256


void tmr_setup_period(int timer, int ms) {
    unsigned int prescaler_values[] = {1, 8, 64, 256};  // Supported prescalers
    unsigned int prescaler_bits[] = {0, 1, 2, 3};       // Corresponding TCKPS values
    unsigned int prescaler = 1;
    unsigned int tckps = 0;
    unsigned long timer_count;                          // 32-bits
    
    for (int i = 0; i < 4; i++) {
        prescaler = prescaler_values[i];
        tckps = prescaler_bits[i];

        timer_count = (FCY / 1000) * ms / prescaler;    // Convert ms to timer ticks

        if (timer_count <= 65535) {     // If fits within 16-bit PR1 register
            break;                      // Stop searching, this is the best prescaler
        }
        // Otherwise if no prescaler value is feasible, exit the function
        else if (timer_count > 65535 && prescaler == 256) return;   
    }
    
    switch(timer) {
        case TIMER1:
            T1CONbits.TON = 0;          // Stop Timer1
            IFS0bits.T1IF = 0;          // Clear Timer1 interrupt flag
            TMR1 = 0;                   // Reset Timer1 counter
            PR1 = timer_count;          // Set timer period
            T1CONbits.TCKPS = tckps;    // Set selected prescaler
            T1CONbits.TON = 1;          // Start Timer1
            break;
        case TIMER2:
            T2CONbits.TON = 0;          // Stop Timer2
            IFS0bits.T2IF = 0;          // Clear Timer2 interrupt flag
            TMR2 = 0;                   // Reset Timer2 counter
            PR2 = timer_count;          // Set timer period
            T2CONbits.TCKPS = tckps;    // Set selected prescaler
            T2CONbits.TON = 1;          // Start Timer2
            break;
        default:
            return;
    }
}


int tmr_wait_period(int timer) {
    int expired = 0;
    
    switch (timer) {
        case TIMER1:
            if (IFS0bits.T1IF) {
                expired = 1;            // Expired before function call
            }
            while (!IFS0bits.T1IF) {     
                // Wait for Timer1 flag
            }
            IFS0bits.T1IF = 0;          // Clear Timer1 flag
            break;                   

        case TIMER2:
            if (IFS0bits.T2IF) {
                expired = 1;            // Expired before function call
            }
            while (!IFS0bits.T2IF) {     
                // Wait for Timer2 flag
            }
            IFS0bits.T2IF = 0;          // Clear Timer2 flag
            break;             
    }
    return expired;
}


void tmr_wait_ms(int timer, int ms){  
    // If the requested delay is less than the MAX_DELAY obtainable with a 
    // a single 16 bit timer, directly setup and wait for that ms value.
    // (The setup function will automatically set the lowest feasible prescaler)
    if (ms <= MAX_DELAY) {
        tmr_setup_period(timer, ms);
        tmr_wait_period(timer);
        // Stop the timer
        switch (timer) {
            case TIMER1:
                T1CONbits.TON = 0;
                break;
            case TIMER2:
                T2CONbits.TON = 0;
                break;
            default:
                break;
        }
    } 
    // If the requested delay is bigger than the MAX_DELAY obtainable, we need
    // to split the wait time into multiple big chunks with the prescaler set to 
    // 256 (automatically done by tmr_setup_period because MAX_DELAY is feasible
    // only with the max prescaler). For the remaining time, we call the setup 
    // again which chooses the optimal prescaler for it.
    else {
        int n_cycles = ms / MAX_DELAY;
        int remainder = ms % MAX_DELAY;
        // Wait for the necessary number of big delay chunks
        tmr_setup_period(timer, MAX_DELAY);
        for (int i = 0; i < n_cycles; i++) {
            tmr_wait_period(timer);
        }
        // Wait for the remaining time 
        tmr_setup_period(timer, remainder);
        tmr_wait_period(timer);
        // Stop the timer
        switch (timer) {
            case TIMER1:
                T1CONbits.TON = 0;
                break;
            case TIMER2:
                T2CONbits.TON = 0;
                break;
            default:
                break;
        }
    }
}