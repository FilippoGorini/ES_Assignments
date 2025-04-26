#include <xc.h>
#include "uart_lib.h"
#include "general_purpose_lib.h"


void uart_init() {
    // Remap UART1 pins to use the MikroBUS 2
    RPOR0bits.RP64R = 0x01;             // Map UART1 TX to RD0
    RPINR18bits.U1RXR = 0x4b;           // Map UART1 RX to RD11
    
    // UART mode configuration (these are default values already, put them just for completeness)
    U1MODEbits.BRGH = 0;                // Standard speed mode (16x)
    U1MODEbits.PDSEL = 0b00;            // 8-bit data, no parity
    U1MODEbits.STSEL = 0;               // 1 stop bit
    
    // Set baud rate to UART_BAUD_RATE(9600 bps)
    // Because of truncation, we get an actual baud rate of circa 9615.38 bps
    // This is a 0.16% error, and UART usually has a 2% tolerance, so we're good
    U1BRG = (unsigned int) ((FCY / (16UL * UART_BAUD_RATE)) - 1);
    
    // Set uart interrupt triggering mode
    uart_rx_interrupt_mode_set();       
    uart_tx_interrupt_mode_set();
    
    U1MODEbits.UARTEN = 1;              // Enable UART1
    U1STAbits.UTXEN = 1;                // Enable U1TX (this also lifts the U1TXIF flag!)
}

void uart_rx_interrupt_mode_set() {
    U1STAbits.URXISEL1 = 0;     // Set rx int. mode to receive an interrupt ...
    U1STAbits.URXISEL0 = 0;     // ... everytime a byte is received
}

void uart_tx_interrupt_mode_set() {
    U1STAbits.UTXISEL1 = 0;     // Set tx int. mode to send an interrupt every time ...
    U1STAbits.UTXISEL0 = 0;     // ... a byte is transferred into the TSR (at least 1 free slot in the FIFO)
}

int uart_send_string(CircularBuffer* tx_buf_ptr, const char* str_ptr) {
    int missed_bytes = 0;
    // Iterate on every character of the string and add it to the txBuffer
    while (*str_ptr) {
        // We create a critical section disabling the interrupt around the ...
        // ... Buffer_Write to protect it from interrupts
        uart_tx_interrupt_disable();
        if (Buffer_Write(tx_buf_ptr, *str_ptr) == -1) {         // Non-blocking write
            missed_bytes ++;
        }
        uart_tx_interrupt_enable();
        // During the first call to uart_send_string, the UTXEN in the init already ...
        // ... lifted the flag U1TXIF, so the ISR will be triggered as soon as we enable ...
        // ... the interrupt for the first time
        str_ptr++;
    }
    return missed_bytes;
}