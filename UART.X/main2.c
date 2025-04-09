#include "xc.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/general_purpose_lib.h"
#include "ring_buffer.h"
#include <string.h>




int received_chars = 0;
char data = '0';
int ld2_enabled = 1;  // Variable to enable/disable LD2 blinking


CircularBuffer_0 buffer;
CircularBuffer_0 *cb = &buffer;

int state = 0;


int parser_uart(CircularBuffer_0* cb, int state){
    if (Buffer_Read_0(cb, &data) == 0){
        if (data == 'L'){
            state = 1;
            return parser_uart(cb, state);
        }
        if (state == 1 && data == 'D'){
            state = 2;
            return parser_uart(cb, state);
        }
        if (state == 2 && data == '1'){
            state = 0;
            return 1;
        }
        if (state == 2 && data == '2'){
            state = 0;
            return 2;
        }
        else {
            state = 0;
            return -1; 
        }
        
    }
    return 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;  // Clear Timer3 Interrupt Flag
    if (ld2_enabled) {
        LED2 = !LED2;  // Toggle LED2 only if enabled
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    char received_char = U1RXREG;  // Read received character
    IFS0bits.U1RXIF = 0;  // Clear interrupt flag
    received_chars++; // Increment received character counter

    // Store character in circular buffer
    if (Buffer_Write_0(cb, received_char) == -1){
        //do something
        parser_uart(cb, state);
    }
}



int main(void) {
    int ret;
    int tag = 0;
    
    
    Buffer_Init_0(cb);
    
    leds_init();
    
    // SETUP INTERRUPTS
    global_interrupt_enable();
    IEC0bits.T3IE = 1;  // Enable Timer3 interrupt
    uart_interrupt_enable();
    
    // SETUP UART
    uart_init();
    
    // SETUP TIMERS
    tmr_setup_period(TIMER1, 10);  // Set TIMER1 to 100 Hz
    tmr_setup_period(TIMER3, 200);  // Set TIMER3 to blink LED2 at 2.5Hz
    
    while (1) {
        algorithm();
        tag = parser_uart(cb, state);
        switch (tag){
            case 1:
                LED1 = !LED1;
                break;
            case 2:
                ld2_enabled = !ld2_enabled;
                break;
        }
        ret = tmr_wait_period(TIMER1);
    }
    
    
}