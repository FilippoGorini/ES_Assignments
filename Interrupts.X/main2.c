#include <xc.h>
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/general_purpose_lib.h"


void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    IFS1bits.INT1IF = 0;        // Reset INT1 interrupt flag
    IEC0bits.T2IE = 1;
    tmr_setup_period(TIMER2, 10);
}

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt() {
    IFS0bits.T2IF = 0;        // Reset INT1 interrupt flag
    IEC0bits.T2IE = 0;
    T2CONbits.TON = 0;
    IEC1bits.INT1IE = 1;
    
    if (BUTTON2 == 1) {
        LED2 = !LED2;
    }
}

int main(void) {
    
    set_digital_mode();
    leds_init();
    buttons_init();
 
    global_interrupt_enable();
    RPINR0bits.INT1R = 0x59;    // Associate INT1 functionality to RE9 (RP89=0x59)
    IFS1bits.INT1IF = 0;        // Clear flag
    IEC1bits.INT1IE = 1;        // Enable specific interrupt

    tmr_setup_period(TIMER1, 200);
    while(1){
        LED1 = !LED1;
        tmr_wait_period(TIMER1);
    }

    return 0;
}