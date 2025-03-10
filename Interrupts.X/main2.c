/*
 * File:   main2.c
 * Author: smuss
 *
 * Created on 06 March 2025, 17:11
 */


#include "xc.h"
#include "timer.h"


#define  LED2 TRISGbits.TRISG9


int main(void) {
    
    TRISGbits.TRISG9 = 0;
    
    LATGbits.LATG9 = 0;
    
    while(1){
        tmr_wait_ms(TIMER1, 200);
        LED2 = !LED2;
        
    }
    
    return 0;
}
