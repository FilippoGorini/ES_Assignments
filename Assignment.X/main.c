#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"
#include "../LIBRARY.X/parser_lib.h"


volatile char rx_buffer_array[RX_BUFFER_SIZE];
volatile CircularBuffer rxBuffer;
volatile char tx_buffer_array[TX_BUFFER_SIZE];
volatile CircularBuffer txBuffer;
MagDataBuffer magBuffer;


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    
    IFS0bits.U1RXIF = 0;                // Clear interrupt flag
    char received_byte = U1RXREG;       // Read received character
    if (Buffer_Write(&rxBuffer, received_byte) == -1) {
        // OVERFLOW ERROR
    }
}


void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
    
    IFS0bits.U1TXIF = 0;
    char byte;
    if (Buffer_Read(&txBuffer, &byte) == 0) {
        U1TXREG = byte;
    } else {
        // If txBuffer is empty, there are no more characters to be sent for now so ...
        // ... we disable the tx interrupt. It will be reenabled by the uart_send_string ...
        // ... function once new characters are ready to be sent
        uart_tx_interrupt_disable();
    }
}


int main(void) {
    
//    int ret = 0;
    unsigned int count_led = 0;             // Counter to toggle LED2    
    unsigned int count_mag_fb = 0;          // Counter to manage mag fb rate to the uart
    unsigned int count_mag_read = 0;        // Counter to manage mag reading
    unsigned int count_yaw_fb = 5;          // Counter to manage yaw feedback
    unsigned int rate_mag_fb = 5;           // Magnetometer feedback rate [Hz]
    unsigned int cycles_mag_fb = 100 / rate_mag_fb;
    char rx_byte; 
    
    // Initialize circular buffers
    Buffer_Init(&rxBuffer, rx_buffer_array, RX_BUFFER_SIZE);        // Initialize RX buffer
    Buffer_Init(&txBuffer, tx_buffer_array, TX_BUFFER_SIZE);        // Initialize TX buffer
    MagDataBuffer_Init(&magBuffer);
    
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
    IFS0bits.U1RXIF = 0;        // Clear UART1 RX interrupt flag
    uart_rx_interrupt_enable();
    // NB: We don't enable the tx interrupt yet as it will be done by uart_send_string
 
    // Setup timers
    tmr_setup_period(TIMER1, 10);           // Setup TIMER1 to time main loop at 100 Hz

    while (1) {
        // Waste 7 ms
        algorithm(); 
        
        // This loop parses one by one all the new, unread bytes in the rxBuffer
        // If a "RATE" message is detected, the rate_mag_fb is updated
        // If an invalid rate is detected, a error message is sent on the uart
        while (Buffer_Read(&rxBuffer, &rx_byte) == 0) {
            // Feed each received byte into the parser.
            if (parse_byte(&ps, rx_byte) == NEW_MESSAGE) {          // We have a complete message.
                if (strcmp(ps.msg_type, "RATE") == 0) {             // Check if it is the $RATE command.
                    int new_rate = extract_integer(ps.msg_payload);  
                    if ((new_rate == 0) ||
                        (new_rate == 1) || 
                        (new_rate == 2) || 
                        (new_rate == 4) || 
                        (new_rate == 5) || 
                        (new_rate == 10)) {
                        rate_mag_fb = new_rate;                     // Update magnetometer feedback rate 
                        if (rate_mag_fb > 0) {
                            cycles_mag_fb = 100 / rate_mag_fb;
                        }
                        LEDBRAKE = 0;           /////// DEBUG ///////
                    } 
                    else {
                        LEDBRAKE = 1;           /////// DEBUG ///////
                        uart_send_string(&txBuffer, "$ERR,1*\n");
                    }
                }
            }
        }
        
        // Handle magnetometer reading at 25 Hz
        if (count_mag_read == 0) {
            mag_update_readings(&magBuffer);
        }
        count_mag_read = (count_mag_read + 1) % 4;
        
        // Handle magnetometer uart feedback at <rate_mag_fb> Hz
        if (rate_mag_fb > 0) {
            if (count_mag_fb == 0) {
                int avg_x, avg_y, avg_z;
                MagDataBuffer_Average(&magBuffer, &avg_x, &avg_y, &avg_z);  // Get average readings
                char msg[32];
                sprintf(msg, "$MAG,%d,%d,%d*\n", avg_x, avg_y, avg_z);    // Format message
                uart_send_string(&txBuffer, msg);
                LEDL = !LEDL;                   /////// DEBUG ///////
                LEDR = !LEDR;                   /////// DEBUG ///////
            }
            count_mag_fb = (count_mag_fb + 1) % cycles_mag_fb;   
        }
        
        // Handle yaw feedback at 5 Hz
        if (count_yaw_fb == 0) {
            int avg_x, avg_y, avg_z;
            int angle_north;
            MagDataBuffer_Average(&magBuffer, &avg_x, &avg_y, &avg_z); 
            angle_north = (atan2(avg_y, avg_x) * 180) / M_PI;
            char msg[16];
            sprintf(msg, "$YAW,%d*\n", angle_north); 
            uart_send_string(&txBuffer, msg);
        }
        count_yaw_fb = (count_yaw_fb + 1) % 20;
        
        // Handle LED2 blinking at 1 Hz
        if (count_led == 0) {
            LED2 = !LED2;
        }
        count_led = (count_led + 1) % 50;
        
        // Wait until next period
        if (tmr_wait_period(TIMER1) == 1) {     // If expired
            LED1 = 1;                           /////// DEBUG /////// 
        }
        else {                                  // If not expired
            LED1 = 0;                           /////// DEBUG /////// 
        }
    }

    return 0;
}
