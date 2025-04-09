#include <xc.h>
#include <string.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"
#include "../LIBRARY.X/parser_lib.h"


char rx_buffer_array[RX_BUFFER_SIZE];
volatile CircularBuffer rxBuffer;


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;                // Clear interrupt flag
    char received_char = U1RXREG;       // Read received character
    
    if (Buffer_Write(&rxBuffer, received_char) == -1) {
        // OVERFLOW ERROR
    }
}


int main(void) {
    
    int ret = 0;
    int i = 0;                          // Counter to toggle LED2
    char rx_byte;                          
    
    // Initialize parser
    parser_state ps;
	ps.state = STATE_DOLLAR;
	ps.index_type = 0; 
	ps.index_payload = 0;

    // Initialize circular buffers
    if (Buffer_Init(&rxBuffer, rx_buffer_array, RX_BUFFER_SIZE) != 0) {      // Initialize rx buffer
        // ALLOCATION ERROR
    }

    leds_init();
    set_digital_mode();
    global_interrupt_enable();
    uart_init();
    uart_interrupt_enable();

    tmr_setup_period(TIMER1, 10);

    while (1) {
        // Waste 7 ms
        algorithm(); 
        
        // This loop parses one by one all the new, unread bytes in the rxBuffer
        while (Buffer_Read(&rxBuffer, &rx_byte) == 0) {
            // Feed each received byte into the parser.
            if (parse_byte(&ps, rx_byte) == NEW_MESSAGE) {
                // We have a complete message.
                if (strcmp(ps.msg_type, "RATE") == 0) {                 // Check if it is the $RATE command.
                    int new_rate = extract_integer(ps.msg_payload);     // Convert payload string to integer.
                    if ((new_rate == 0) || (new_rate == 1) || (new_rate == 2) ||
                        (new_rate == 4) || (new_rate == 5) || (new_rate == 10)) {
                        // Update magnetometer feedback rate.
                        LED1 = !LED1;
                    } else {
                        // Invalid command: send error message.
                    }
                }
            }
        }
        
        // Handle LED2 toggling
        if (i == 0) {
            LED2 = !LED2;
        }
        i = (i + 1) % 50;
        
        // Wait until next period
        ret = tmr_wait_period(TIMER1);
    }

    return 0;
}
