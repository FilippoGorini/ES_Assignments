#include <xc.h>
#include "pwm_lib.h"


void oc_init(void) {
    OC1CON1bits.OCTSEL = 7;     // Use internal clock (FCY))
    OC2CON1bits.OCTSEL = 7;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.SYNCSEL = 0x1F;   
    OC1RS = PWM_PERIOD;         // Set 10 kHz pwm period
    OC2RS = PWM_PERIOD; 
}

void oc_start(void) {
    OC1CON1bits.OCM = 0b111;
    OC2CON1bits.OCM = 0b111;
}

void oc_stop(void) {
    OC1CON1bits.OCM = 0;
    OC2CON1bits.OCM = 0;
}

void oc1_set_duty(unsigned int duty_percent) { 
    OC1R  = (PWM_PERIOD * duty_percent) / 100;  
}

void oc2_set_duty(unsigned int duty_percent) {  
    OC2R  = (PWM_PERIOD * duty_percent) / 100;
}

//void oc_pins_init(void) {
////    // RD1
////    RPOR0bits.RP65R = 0b010000;     // Remap
////    TRISDbits.TRISD1 = 0;           // Set as output
//
//    // RD2
//    RPOR1bits.RP66R = 0b010001;
//    TRISDbits.TRISD2 = 0;
//
////    // RD3
////    RPOR1bits.RP67R = 0b010010;
////    TRISDbits.TRISD3 = 0;
//
//    // RD4
//    RPOR2bits.RP68R = 0b010011;
//    TRISDbits.TRISD4 = 0;
//}























//void setupPWM() {
//    OC1CON1bits.OCTSEL = 0b111; // Select system clock
//    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
//    OC1CON2bits.SYNCSEL = 0x1F; // Internal sync  (11111 in binary)
//
//    OC1RS = PWM_PERIOD; // Set PWM period
//    OC1R = PWM_DUTY;    // Set initial duty
//}
//
//void startPWM() {
//    OC1CON1bits.OCM = 0b110; 
//}
//
//void stopPWM() {
//    OC1CON1bits.OCM = 0b000; 
//}
//
//
//void setupPWM_OC1_OC2() {
//    // Remapping OC1 ? RP7 (motore sinistro avanti)
//    //RPOR3bits.RP7R = 18; // 18 = OC1
//    RPOR3bits.RP70R = 18;
//    TRISBbits.TRISB7 = 0;
//
//    // Remapping OC2 ? RP8 (motore destro avanti)
//    //RPOR4bits.RP8R = 19; // 19 = OC2
//    RPOR4bits.RP80R = 19;
//    TRISBbits.TRISB8 = 0;
//
//    // Configura OC1 (sinistra)
//    OC1CON1bits.OCTSEL = 0b111; // System clock
//    OC1CON2bits.SYNCSEL = 0x1F;
//    OC1RS = PWM_PERIOD;
//    OC1R = PWM_DUTY;
//    OC1CON1bits.OCM = 0b110;
//
//    // Configura OC2 (destra)
//    OC2CON1bits.OCTSEL = 0b111; // System clock
//    OC2CON2bits.SYNCSEL = 0x1F;
//    OC2RS = PWM_PERIOD;
//    OC2R = PWM_DUTY;
//    OC2CON1bits.OCM = 0b110;
//
//    // Disattiva backward (motori indietro)
//    LATBbits.LATB2 = 0;  // sinistra indietro
//    TRISBbits.TRISB2 = 0;
//
//    LATBbits.LATB4 = 0;  // destra indietro
//    TRISBbits.TRISB4 = 0;
//}



