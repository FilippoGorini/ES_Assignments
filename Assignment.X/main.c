#include "xc.h"
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"


void __attribute__((__interrupt__, no_auto_psv)) _T5Interrupt(void) {
    IFS1bits.T5IF = 0;      // Clear Timer3 Interrupt Flag
    LED2 = !LED2;          
}


int main() {
    
    int ret = 0;
    
    leds_init();
    set_digital_mode();
    global_interrupt_enable();
    
    IFS1bits.T5IF = 0;
    IEC1bits.T5IE = 1;  // Enable Timer3 interrupt
    
    tmr_setup_period(TIMER1, 10);
    tmr32_setup_period(TIMER4, 500);
    
    while(1) {
        algorithm();
        // code to handle the assignment
        ret = tmr_wait_period(TIMER1);
    }
    return 0;
}
