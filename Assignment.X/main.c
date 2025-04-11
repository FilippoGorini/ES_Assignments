#include <xc.h>
#include <string.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"
#include "../LIBRARY.X/parser_lib.h"


volatile char rx_buffer_array[RX_BUFFER_SIZE];
volatile CircularBuffer rxBuffer;
//volatile char tx_buffer_array[TX_BUFFER_SIZE];
//volatile CircularBuffer txBuffer;


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;                // Clear interrupt flag
    char received_char = U1RXREG;       // Read received character
    
    if (Buffer_Write(&rxBuffer, received_char) == -1) {
        // OVERFLOW ERROR
    }
}


//void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
//    IFS0bits.U1TXIF = 0;
//    
//    char c;
//    if (Buffer_Read(&txBuffer, &c) == 0) {
//        U1TXREG = c;
//    } else {
//        // Buffer is empty, disable TX interrupt to avoid constant triggering
//        // (Actually it can't happen if there are no more bytes to be sent even at the next isr call)
//        IEC0bits.U1TXIE = 0; // ???????????
//    }
//}


int main(void) {
    
    int ret = 0;
    int count_led = 0;                  // Counter to toggle LED2    
    int count_mag_fb = 0;               // Counter to manage mag fb rate to the uart
    int mag_fb_rate = 5;                // Magnetometer feedback rate [Hz]
    int mag_fb_cycles = 100 / mag_fb_rate;
    char rx_byte;       
    
    // Initialize uart circular buffers
    if (Buffer_Init(&rxBuffer, rx_buffer_array, RX_BUFFER_SIZE) != 0) {      // Initialize rx buffer
        // ALLOCATION ERROR
    }
//    if (Buffer_Init(&txBuffer, tx_buffer_array, TX_BUFFER_SIZE) != 0) {      // Initialize rx buffer
//        // ALLOCATION ERROR
//    }
    
    // Initialize parser
    parser_state ps;
	ps.state = STATE_DOLLAR;
	ps.index_type = 0; 
	ps.index_payload = 0;

    // Initialize peripherals
    set_digital_mode();
    leds_init();
    lights_init();
    uart_init();
    spi_init();
    mag_sus2act();
    
    // Setup interrupts
    global_interrupt_enable();
    uart_rx_interrupt_enable();
//    uart_tx_interrupt_enable();

    tmr_setup_period(TIMER1, 10);           // Setup TIMER1 to time main loop at 100 Hz

    while (1) {
        // Waste 7 ms
        algorithm(); 
        
        // This loop parses one by one all the new, unread bytes in the rxBuffer
        while (Buffer_Read(&rxBuffer, &rx_byte) == 0) {
            // Feed each received byte into the parser.
            if (parse_byte(&ps, rx_byte) == NEW_MESSAGE) {              // We have a complete message.
                if (strcmp(ps.msg_type, "RATE") == 0) {                 // Check if it is the $RATE command.
                    int new_rate = extract_integer(ps.msg_payload);     // Convert payload string to integer.
                    if ((new_rate == 0) ||
                        (new_rate == 1) || 
                        (new_rate == 2) || 
                        (new_rate == 4) || 
                        (new_rate == 5) || 
                        (new_rate == 10)) {
                        mag_fb_rate = new_rate;         // Update magnetometer feedback rate 
                        mag_fb_cycles = 100 / mag_fb_rate;
//                        count_mag_fb = 0;               // Reset counter
                        LEDBRAKE = 0;   /////// DEBUG ///////
                    } 
                    else {
                        LEDBRAKE = 1;   /////// DEBUG ///////
                        // Invalid command: send error message.
                    }
                }
            }
        }
        
        // Handle magnetometer uart feedback
        if (mag_fb_rate > 0) {
            if (count_mag_fb == 0) {
                LEDFRONT = !LEDFRONT;   /////// DEBUG ///////
                // - average last 5 measurements
                // - format message: $MAG,x,y,z*
                // - write message on the txBuffer
            }
            count_mag_fb = (count_mag_fb + 1) % mag_fb_cycles;   
        }
        
        // Handle LED2 toggling
        if (count_led == 0) {
            LED2 = !LED2;
        }
        count_led = (count_led + 1) % 50;
        
        // Wait until next period
        ret = tmr_wait_period(TIMER1);
        if (ret == 1) {     /////// DEBUG ///////
            LED1 = 1;   
        }
    }

    return 0;
}
