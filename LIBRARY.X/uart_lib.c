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
    U1STAbits.UTXEN = 1;                // Enable U1TX (transmission) 
}

void uart_rx_interrupt_mode_set() {
    U1STAbits.URXISEL1 = 0;     // Set rx int. mode to receive an interrupt ...
    U1STAbits.URXISEL0 = 0;     // ... everytime a byte is received
}

void uart_tx_interrupt_mode_set() {
    U1STAbits.UTXISEL1 = 0;     // Set tx int. mode to send an interrupt every time ...
    U1STAbits.UTXISEL0 = 0;     // ... a byte is transferred into the TSR (at least 1 free slot in the FIFO)
}

void uart_rx_interrupt_enable() {
    IFS0bits.U1RXIF = 0;        // Clear UART1 RX interrupt flag
    IEC0bits.U1RXIE = 1;        // Enable UART1 RX interrupt
}

void uart_rx_interrupt_disable() {
    IEC0bits.U1RXIE = 0;
}

void uart_tx_interrupt_enable() {
    IFS0bits.U1TXIF = 0;        // Clear UART1 TX interrupt flag
    IEC0bits.U1TXIE = 1;        // Enable UART1 TX interrupt
}

void uart_tx_interrupt_disable() {
    IEC0bits.U1TXIE = 0;
}

void uart_send_string(CircularBuffer* tx_buf_ptr, const char* str_ptr) {
    while (*str_ptr) {
        while (Buffer_Write(tx_buf_ptr, *str_ptr) == -1);  // Wait if full
        str_ptr++;
    }

    // Manually transmit the first byte of the string in order to kickstart the ...
    // ... transmission of the next bytes trough the ISR
    if (!IEC0bits.U1TXIE) {
        if (!U1STAbits.UTXBF) {
            char byte;
            if (Buffer_Read(tx_buf_ptr, &byte) == 0) {
                // NB: Here order is important: swapping the next two lines results ...
                // ... in the the interrupt flag being set before the call of the ...
                // ... uart_tx_interrupt_enable function, which resets it before ...
                // ... enabling the interrupts, so the ISR never fires. This also results in the main loop getting stuck!
                uart_tx_interrupt_enable();
                U1TXREG = byte;
            }
        }
    }
}