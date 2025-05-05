#include <xc.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/pwm_lib.h"
#include "../LIBRARY.X/timer_lib.h"

int main(void) {

    set_digital_mode();
    oc_init();
    oc_pins_init();
    oc1_set_duty(0);
    oc2_set_duty(0);
    oc3_set_duty(0);
    oc4_set_duty(0);
    oc_start();
    
    
    // Test all movements in sequence
    while(1){
        move_forward();
        tmr_wait_ms(TIMER1, 3000);
        move_backward();
        tmr_wait_ms(TIMER1, 3000);
        rotate_right();
        tmr_wait_ms(TIMER1, 3000);
        rotate_left();
        tmr_wait_ms(TIMER1, 3000);
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
