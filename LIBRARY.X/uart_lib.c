#include <xc.h>
#include "uart_lib.h"


void uart_init() {
    // Remap UART1 pins to use the MikroBUS 2
    RPOR0bits.RP64R = 0x01;             // Map UART1 TX to RD0
    RPINR18bits.U1RXR = 0x4b;           // Map UART1 RX to RD11
    // Set baud rate to 9600
    // Because of truncation, we get an actual baud rate of circa 9615.38 bps
    // This is a 0.16% error, and UART usually has a 2% tolerance, so we're good
    U1BRG = (unsigned int) ((FCY / (16UL * UART_BAUD_RATE)) - 1);
    
    U1MODEbits.UARTEN = 1;              // Enable UART1
    U1STAbits.UTXEN = 1;                // Enable U1TX (transimission) 
}

void uart_interrupt_enable() {
    IFS0bits.U1RXIF = 0;  // Clear UART1 RX interrupt flag
    IEC0bits.U1RXIE = 1;  // Enable UART1 RX interrupt
}