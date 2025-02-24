/*
 * File:   MainExercise1.c
 * Author: Lenovo
 *
 * Created on February 24, 2025, 9:27 AM
 */


#include <xc.h>
#include <stdbool.h>

int main(void) {
    // Disable analog mode 
    ANSELEbits.ANSE8 = 0;
    // Set pin A0 as output (led)
    TRISAbits.TRISA0 = 0;
    // Set pin E8 as input (button)
    TRISEbits.TRISE8 = 1;
    
    bool prev_button_state = 1      // Button is assumed to be normally high (not pressed)
    bool led_state = 0;             // Initial LED state (off)

    while (1) {
        if (prev_button_state == 1 && PORTEbits.RE8 == 0) { 
            // Button was released and now is pressed (rising edge detected)
            led_state = !led_state;     // Toggle the LED state
            LATAbits.LATA0 = led_state; // Apply new LED state
        }
        
        prev_button_state = PORTEbits.RE8;  // Update previous button state
    }
    
    return 0;
}