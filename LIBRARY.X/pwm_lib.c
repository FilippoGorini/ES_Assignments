#include "xc.h"
#include "pwm_lib.h"


#define Fcy 70000000UL           // Frequenza del sistema in Hz
#define T_pwm 0.0001             // 100 µs = 10 kHz
#define PWM_PERIOD (unsigned int)(T_pwm * Fcy)   // 7000
#define PWM_DUTY (unsigned int)((40 * PWM_PERIOD) / 100) // 2800


void setupPWM() {
    OC1CON1bits.OCTSEL = 0b111; // Select system clock
    OC1CON1bits.OCM = 0b110;    // Edge-aligned PWM mode
    OC1CON2bits.SYNCSEL = 0x1F; // Internal sync  (11111 in binary)

    OC1RS = PWM_PERIOD; // Set PWM period
    OC1R = PWM_DUTY;    // Set initial duty
}

void startPWM() {
    OC1CON1bits.OCM = 0b110; 
}

void stopPWM() {
    OC1CON1bits.OCM = 0b000; 
}


void setupPWM_OC1_OC2() {
    // Remapping OC1 ? RP7 (motore sinistro avanti)
    //RPOR3bits.RP7R = 18; // 18 = OC1
    RPOR3bits.RP70R = 18;
    TRISBbits.TRISB7 = 0;

    // Remapping OC2 ? RP8 (motore destro avanti)
    //RPOR4bits.RP8R = 19; // 19 = OC2
    RPOR4bits.RP80R = 19;
    TRISBbits.TRISB8 = 0;

    // Configura OC1 (sinistra)
    OC1CON1bits.OCTSEL = 0b111; // System clock
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1RS = PWM_PERIOD;
    OC1R = PWM_DUTY;
    OC1CON1bits.OCM = 0b110;

    // Configura OC2 (destra)
    OC2CON1bits.OCTSEL = 0b111; // System clock
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2RS = PWM_PERIOD;
    OC2R = PWM_DUTY;
    OC2CON1bits.OCM = 0b110;

    // Disattiva backward (motori indietro)
    LATBbits.LATB2 = 0;  // sinistra indietro
    TRISBbits.TRISB2 = 0;

    LATBbits.LATB4 = 0;  // destra indietro
    TRISBbits.TRISB4 = 0;
}




int main(void) {
    int PWMstate = 0;
    int lastButtonState = 0;

    setupPWM();
    setupPWM_OC1_OC2();

    while (1) {
        int currentButton = PORTEbits.RE8;

        if (currentButton && !lastButtonState) {
            if (PWMstate == 0) {
                startPWM();
                PWMstate = 1;
            } else {
                stopPWM();
                PWMstate = 0;
            }

            __delay_ms(50);
        }

        lastButtonState = currentButton;
    }

    return 0;
}

