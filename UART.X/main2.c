#include "xc.h"
#include "timer.h"

#define FCY 72000000UL
#define BAUD_RATE 9600
#define LED1 LATAbits.LATA0
#define LED2 LATGbits.LATG9


void algorithm() {
    tmr_wait_ms(TIMER2, 7);             // Waste 7 ms to simulate algorithm
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;                  // Clear Timer3 Interrupt Flag
    LED2 = !LED2;                       // Toggle LED2
}

int main(void) {
    int ret;
    
    TRISAbits.TRISA0 = 0;               // Set RA0 as output (LED1)
    TRISGbits.TRISG9 = 0;               // Set RG9 as output (LED2)
    // SETUP INTERRUPTS
    INTCON2bits.GIE = 1;                // Enable global interrupt
    IEC0bits.T3IE = 1;                  // Enable Timer3 interrupt
    // SETUP TIMERS
    tmr_setup_period(TIMER1, 10);       // Set TIMER1 to 100 Hz
    tmr_setup_period(TIMER3, 200);      // Set TIMER3 to blink LED2 at 2.5Hz
    // SETUP UART
    RPOR0bits.RP64R = 0x01;             // Map UART1 TX to RD0
    RPINR18bits.U1RXR = 0x4b;           // Map UART1 RX to RD11
    // Set baud rate to 9600
    // Because of truncation, we get an actual baud rate of circa 9615.38 bps
    // This is a 0.16% error, and UART usually has a 2% tolerance, so we're good
    U1BRG = (unsigned int) ((FCY / (16UL * 9600)) - 1);
    U1MODEbits.UARTEN = 1;              // Enable UART1
    U1STAbits.UTXEN = 1;                // Enable U1TX (transimission)
    
    while(1) {
        algorithm();
        ret = tmr_wait_period(TIMER1);
    }
    
    
    return 0;
}
