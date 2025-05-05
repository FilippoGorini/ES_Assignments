#include <xc.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/pwm_lib.h"
#include "../LIBRARY.X/timer_lib.h"

int main(void) {
    
    int duty_percent_1 = 100;
    int duty_percent_2 = 80;
    
    set_digital_mode();
    
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;

    oc_init();
    oc1_set_duty(duty_percent_1);
    oc2_set_duty(duty_percent_2);
    oc_start();
    
    
    while(1){
        turn_right_forward();
        tmr_wait_ms(TIMER1, 3000);
        turn_left_forward();
        tmr_wait_ms(TIMER1, 3000);
        turn_right_backward();
        tmr_wait_ms(TIMER1, 3000);
        turn_left_backward();
        tmr_wait_ms(TIMER1, 3000);
    }

    return 0;
}
