/*
 * File:   main1.c
 * Author: smuss
 *
 * Created on 06 March 2025, 16:40
 */

#include "timer.h"
#include "xc.h"

#define LED1 LATAbits.LATA0

int main(void) {
    
    TRISGbits.TRISG9 = 0;    
    IEC0bits.T3IE = 1;
    LATGbits.LATG9 = 0;
    
    tmr_setup_period_32(TIMER2, 500);
    
    TRISAbits.TRISA0 = 0;
    
    LED1 = 0;
    
    tmr_setup_period(TIMER1, 200);
    while(1){
        LED2 = !LED2;
        tmr_wait_period(TIMER1);
    }
    
    return 0;
}
