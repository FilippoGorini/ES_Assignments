#include <xc.h>
#include "general_purpose_lib.h"
#include "timer_lib.h"


void set_digital_mode(void) {
    // Sets all analog pins to digital mode
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0;
}

void leds_init(void) {
    // Set output mode for the LED1/2 pins
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    // Make sure leds are turned off
    LED1 = 0;
    LED2 = 0;
}

void global_interrupt_enable(void) {
    INTCON2bits.GIE = 1;       
}

void algorithm (void) {
    // Simulates an algorithm which takes 7 ms to execute
    tmr_wait_ms(TIMER2, 7);
}