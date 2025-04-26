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
unsigned int missed_rx_bytes = 0;
unsigned int missed_tx_bytes = 0;


void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    
    IFS0bits.U1RXIF = 0;                // Clear interrupt flag
    // Read bytes from FIFO until they're available. If by any chance we had more than ...
    // ... one byte in the FIFO this allows to read them all and avoid the risk ...
    // ... of accumulating them until the FIFO overruns
    while (U1STAbits.URXDA) {
        char received_byte = U1RXREG;       // Read received character
        if (Buffer_Write(&rxBuffer, received_byte) == -1) {
            // If the rxBuffer is full, the received_byte is lost, so we could ...
            // ... increase a counter to keep track of how many bytes we lost
            missed_rx_bytes ++;
        }
        // As soon as there is no more data available in the FIFO we exit the loop and the ISR
    }
}


void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
    
    IFS0bits.U1TXIF = 0;
    char byte;
    while (!U1STAbits.UTXBF) {
        if (Buffer_Read(&txBuffer, &byte) == 0) {
            U1TXREG = byte;
        } else {
            uart_tx_interrupt_disable();
            // If txBuffer is empty, there are no more characters to be sent for now so ...
            // ... we disable the tx interrupt. It will be reenabled by the uart_send_string ...
            // ... function once new characters are added to the txBuffer
            break;  // Exit the loop to avoid getting stuck in a infinite loop
        }
    }
}


int main(void) {
    
    unsigned int count_led = 0;             // Counter to toggle LED2    
    unsigned int count_mag_fb = 2;          // Counter to manage mag fb rate to the uart
    unsigned int count_mag_read = 3;        // Counter to manage mag reading
    unsigned int count_yaw_fb = 0;          // Counter to manage yaw feedback
    // With this initial offsets, we ensure that the magnetometer reading, feedback ...
    // ... and yaw feedback never happen in the same loop (at least until we change ...
    // ... the rate of the magnetometer feedback). Furthermore, the yaw and mag feedback is calculated ...
    // ... for the first time after the first 5 samples to average are acquired.
    // ... NB: as of now, after we chan$MISS*ge the rate we could be unlucky and have the mag ...
    // ... reading and feedback happen in the same loop, anyway this doesn't seem ...
    // ... to cause missed deadlines. A possible improvement would be, whenever we update ...
    // ... the rate, to also reset the counter in such a way that the next feedback happens ...
    // ... exactly one loop after the next magnetometer reading. This would ensure that still ...
    // ... there is no overlap, anyway for now this is overkill.
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
    uart_rx_interrupt_enable();
    // NB: We don't enable the tx interrupt yet as it will be done by uart_send_string once needed
 
    // Setup timers
    tmr_setup_period(TIMER1, 10);           // Setup TIMER1 to time main loop at 100 Hz

    while (1) {
        // Waste 7 ms
        algorithm(); 
        
        // This loop parses one by one all the new, unread bytes in the rxBuffer
        // If a "RATE" message is detected, the rate_mag_fb is updated
        // If an invalid rate is detected, a error message is sent on the uart
        while (1) {
            // Critical section to protect the read buffer operation
            uart_rx_interrupt_disable();
            int read_result = Buffer_Read(&rxBuffer, &rx_byte);
            uart_rx_interrupt_enable();
            
            if (read_result == -1) break;       // Break the loop if the rxBuffer is empty
            
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
                        missed_tx_bytes += uart_send_string(&txBuffer, "$ERR,1*\n");
                    }
                }
                // OPTIONAL: feedback about missed rx/tx bytes when the user ...
                // ... sends the command $MISS*
                else if (strcmp(ps.msg_type, "MISS") == 0) {
                    char msg[32];
                    sprintf(msg, "$MISS,%d,%d*\n", missed_rx_bytes, missed_tx_bytes);  
                    missed_tx_bytes += uart_send_string(&txBuffer, msg);
                }
            }
        }
        
        // Handle magnetometer reading at 25 Hz
        if (++count_mag_read >= 4) {
            count_mag_read = 0;
            mag_update_readings(&magBuffer);
        }
        
        // Handle magnetometer uart feedback at <rate_mag_fb> Hz
        if (++count_mag_fb >= cycles_mag_fb && rate_mag_fb > 0) {
            count_mag_fb = 0;
            int avg_x, avg_y, avg_z;
            MagDataBuffer_Average(&magBuffer, &avg_x, &avg_y, &avg_z);  // Get average readings
            char msg[32];
            sprintf(msg, "$MAG,%d,%d,%d*\n", avg_x, avg_y, avg_z);    // Format message
            missed_tx_bytes += uart_send_string(&txBuffer, msg);
            LEDL = !LEDL;                   /////// DEBUG ///////
            LEDR = !LEDR;                   /////// DEBUG ///////
        }  
        
        // Handle yaw feedback at 5 Hz
        if (++count_yaw_fb >= 20) {
            count_yaw_fb = 0;
            int avg_x, avg_y, avg_z;
            int angle_north;
            MagDataBuffer_Average(&magBuffer, &avg_x, &avg_y, &avg_z); 
            angle_north = (atan2(avg_y, avg_x) * 180) / M_PI;
            char msg[16];
            sprintf(msg, "$YAW,%d*\n", angle_north); 
            missed_tx_bytes += uart_send_string(&txBuffer, msg);
        }
        
        // Handle LED2 blinking at 1 Hz
        if (count_led >= 50) {
            count_led = 0;
            LED2 = !LED2;
        }
        
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
