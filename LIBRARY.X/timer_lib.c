#include <xc.h>
#include "timer_lib.h"


// Supports all timers from T1 to T9
void tmr_setup_period(int timer, int ms) {
    unsigned int prescaler_values[] = {1, 8, 64, 256};  // Supported prescalers
    unsigned int prescaler_bits[] = {0, 1, 2, 3};       // Corresponding TCKPS values
    unsigned int prescaler = 1;
    unsigned int tckps = 0;
    unsigned long long timer_count;                     // 64-bits

    for (int i = 0; i < 4; i++) {
        prescaler = prescaler_values[i];
        tckps = prescaler_bits[i];

        timer_count = ((unsigned long long)FCY * (unsigned long long)ms) / (1000ULL * prescaler);

        if (timer_count <= 0xFFFF) {    // If fits within 16-bit PR1 register (max value 65535)
            break;                      // Stop searching, this is the best prescaler
        }
        // Otherwise if no prescaler value is feasible, exit the function
        else if (i == 3) return;
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
            T2CONbits.TON = 0;        
            IFS0bits.T2IF = 0;         
            TMR2 = 0;                   
            PR2 = timer_count;        
            T2CONbits.TCKPS = tckps;  
            T2CONbits.TON = 1;         
            break;
        case TIMER3:
            T3CONbits.TON = 0;        
            IFS0bits.T3IF = 0;         
            TMR3 = 0;                   
            PR3 = timer_count;        
            T3CONbits.TCKPS = tckps;  
            T3CONbits.TON = 1;         
            break;
        case TIMER4:
            T4CONbits.TON = 0;        
            IFS1bits.T4IF = 0;         
            TMR4 = 0;                   
            PR4 = timer_count;        
            T4CONbits.TCKPS = tckps;  
            T4CONbits.TON = 1;         
            break;
        case TIMER5:
            T5CONbits.TON = 0;        
            IFS1bits.T5IF = 0;         
            TMR5 = 0;                   
            PR5 = timer_count;        
            T5CONbits.TCKPS = tckps;  
            T5CONbits.TON = 1;         
            break;
        case TIMER6:
            T6CONbits.TON = 0;        
            IFS2bits.T6IF = 0;         
            TMR6 = 0;                   
            PR6 = timer_count;        
            T6CONbits.TCKPS = tckps;  
            T6CONbits.TON = 1;         
            break;
        case TIMER7:
            T7CONbits.TON = 0;        
            IFS3bits.T7IF = 0;         
            TMR7 = 0;                   
            PR7 = timer_count;        
            T7CONbits.TCKPS = tckps;  
            T7CONbits.TON = 1;         
            break;
        case TIMER8:
            T8CONbits.TON = 0;        
            IFS3bits.T8IF = 0;         
            TMR8 = 0;                   
            PR8 = timer_count;        
            T8CONbits.TCKPS = tckps;  
            T8CONbits.TON = 1;         
            break;
        case TIMER9:
            T9CONbits.TON = 0;        
            IFS3bits.T9IF = 0;         
            TMR9 = 0;                   
            PR9 = timer_count;        
            T9CONbits.TCKPS = tckps;  
            T9CONbits.TON = 1;         
            break;
        default:
            return;
    }
}


// Supports: TIMER2/3, TIMER4/5, TIMER6/7, TIMER8/9
void tmr32_setup_period(int timer, int ms){
    unsigned int prescaler_values[] = {1, 8, 64, 256};  // Supported prescalers
    unsigned int prescaler_bits[] = {0, 1, 2, 3};       // Corresponding TCKPS values
    unsigned int prescaler = 1;
    unsigned int tckps = 0;
    unsigned long long timer_count;                     // 64-bits

    for (int i = 0; i < 4; i++) {
        prescaler = prescaler_values[i];
        tckps = prescaler_bits[i];

        timer_count = ((unsigned long long)FCY * (unsigned long long)ms) / (1000ULL * prescaler);
        
        if (timer_count <= 0xFFFFFFFF) {    // If fits within 32-bits
            break;                      
        }
        // Otherwise if no prescaler value is feasible, exit the function
        else if (i == 3) return;
    }

    switch(timer) {
        case TIMER2:
            T2CONbits.TON = 0;              // Stop Timer2
            T3CONbits.TON = 0;              // Stop Timer3
            T2CONbits.T32 = 1;              // Enable 32-bit timer mode (T2 + T3)
            T2CONbits.TCKPS = tckps;        // Set selected prescaler
            TMR2 = 0;                       // Reset TMR2 counter
            TMR3 = 0;                       // Reset TMR3 counter
            PR2 = (unsigned int)(timer_count & 0xFFFF);         // Load lower 16 bits into PR2
            PR3 = (unsigned int)((timer_count >> 16) & 0xFFFF); // Load upper 16 bits into PR3
            // We're fine with the interrupt flag's default priority level (4), ...
            // ... otherwise we would need to set it explicitly
            IFS0bits.T3IF = 0;              // Clear Timer3 (higher part) interrupt flag
//            IEC0bits.T3IE = 1;              // Enable Timer3 interrupt
            T2CONbits.TON = 1;              // Start the timer
            break;
        case TIMER4:
            T4CONbits.TON = 0;           
            T5CONbits.TON = 0;            
            T4CONbits.T32 = 1;            
            T4CONbits.TCKPS = tckps;      
            TMR4 = 0;                     
            TMR5 = 0;                       
            PR4 = (unsigned int)(timer_count & 0xFFFF);      
            PR5 = (unsigned int)((timer_count >> 16) & 0xFFFF); 
            IFS1bits.T5IF = 0;             
//            IEC1bits.T5IE = 1;            
            T4CONbits.TON = 1;            
            break;
        case TIMER6:
            T6CONbits.TON = 0;           
            T7CONbits.TON = 0;            
            T6CONbits.T32 = 1;            
            T6CONbits.TCKPS = tckps;      
            TMR6 = 0;                     
            TMR7 = 0;                       
            PR6 = (unsigned int)(timer_count & 0xFFFF);      
            PR7 = (unsigned int)((timer_count >> 16) & 0xFFFF); 
            IFS3bits.T7IF = 0;             
//            IEC3bits.T7IE = 1;            
            T6CONbits.TON = 1;            
            break;
        case TIMER8:
            T8CONbits.TON = 0;           
            T9CONbits.TON = 0;            
            T8CONbits.T32 = 1;            
            T8CONbits.TCKPS = tckps;      
            TMR8 = 0;                     
            TMR9 = 0;                       
            PR8 = (unsigned int)(timer_count & 0xFFFF);      
            PR9 = (unsigned int)((timer_count >> 16) & 0xFFFF); 
            IFS3bits.T9IF = 0;             
//            IEC3bits.T9IE = 1;            
            T8CONbits.TON = 1;            
            break;
        default:
            return;
    }
}


// NOTE: when using two timers together as a 32-bit timer, the argument to this 
// function should be the odd-numbered one (i.e. T3 is using T2/T3)
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
                expired = 1;            
            }
            while (!IFS0bits.T2IF) {
                // Wait for Timer2 flag
            }
            IFS0bits.T2IF = 0;      
            break;
        case TIMER3:
            if (IFS0bits.T3IF) {
                expired = 1;            
            }
            while (!IFS0bits.T3IF) {
                // Wait for Timer3 flag
            }
            IFS0bits.T3IF = 0;      
            break;
        case TIMER4:
            if (IFS1bits.T4IF) {
                expired = 1;            
            }
            while (!IFS1bits.T4IF) {
                // Wait for Timer4 flag
            }
            IFS1bits.T4IF = 0;      
            break;
        case TIMER5:
            if (IFS1bits.T5IF) {
                expired = 1;            
            }
            while (!IFS1bits.T5IF) {
                // Wait for Timer5 flag
            }
            IFS1bits.T5IF = 0;      
            break;
        case TIMER6:
            if (IFS2bits.T6IF) {
                expired = 1;            
            }
            while (!IFS2bits.T6IF) {
                // Wait for Timer6 flag
            }
            IFS2bits.T6IF = 0;      
            break;
        case TIMER7:
            if (IFS3bits.T7IF) {
                expired = 1;            
            }
            while (!IFS3bits.T7IF) {
                // Wait for Timer7 flag
            }
            IFS3bits.T7IF = 0;      
            break;
        case TIMER8:
            if (IFS3bits.T8IF) {
                expired = 1;            
            }
            while (!IFS3bits.T8IF) {
                // Wait for Timer8 flag
            }
            IFS3bits.T8IF = 0;      
            break;
        case TIMER9:
            if (IFS3bits.T9IF) {
                expired = 1;            
            }
            while (!IFS3bits.T9IF) {
                // Wait for Timer9 flag
            }
            IFS3bits.T9IF = 0;      
            break;
        default:
            return -1;
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
            case TIMER3:
                T3CONbits.TON = 0;
                break;
            case TIMER4:
                T4CONbits.TON = 0;
                break;
            case TIMER5:
                T5CONbits.TON = 0;
                break;
            case TIMER6:
                T6CONbits.TON = 0;
                break;
            case TIMER7:
                T7CONbits.TON = 0;
                break;
            case TIMER8:
                T8CONbits.TON = 0;
                break;
            case TIMER9:
                T9CONbits.TON = 0;
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
            case TIMER3:
                T3CONbits.TON = 0;
                break;
            case TIMER4:
                T4CONbits.TON = 0;
                break;
            case TIMER5:
                T5CONbits.TON = 0;
                break;
            case TIMER6:
                T6CONbits.TON = 0;
                break;
            case TIMER7:
                T7CONbits.TON = 0;
                break;
            case TIMER8:
                T8CONbits.TON = 0;
                break;
            case TIMER9:
                T9CONbits.TON = 0;
                break;
            default:
                break;
        }
    }
}