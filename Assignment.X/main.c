#include <xc.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"


volatile CircularBuffer rxBuffer;
CircularBuffer *cb_rx = &rxBuffer;  


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;                // Clear interrupt flag
    char received_char = U1RXREG;       // Read received character

    if (Buffer_Write(&rxBuffer, received_char) == -1) {
        // OVERFLOW ERROR
    }
}


int main(void) {
    
    int ret = 0;
    int i = 0;

    if (Buffer_Init(cb_rx, RX_BUFFER_SIZE) != 0) {      // Initialize rx buffer
        // ALLOCATION ERROR
    }

    leds_init();
    set_digital_mode();
    global_interrupt_enable();
    uart_init();
    uart_interrupt_enable();

    tmr_setup_period(TIMER1, 10);

    while (1) {
        algorithm();  // Your logic

        // Toggle LED2 every 50 iterations (500ms if TIMER1 = 10ms)
        if (i == 0) {
            LED2 = !LED2;
        }
        i = (i + 1) % 50;

        // Read from buffer if needed (e.g., command parsing)
        char byte;
        if (Buffer_Read(cb_rx, &byte) == 0) {
            // Handle received byte (e.g., build commands)
        }

        ret = tmr_wait_period(TIMER1);
    }

    return 0;
}
