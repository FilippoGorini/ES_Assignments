#include "xc.h"

#define FCY 72000000UL
#define BAUD_RATE 9600

int main(void) {
    
    // Remap UART1 pins to use the MikroBUS 2
    RPOR0bits.RP64R = 0x01;             // Map UART1 TX to RD0
    RPINR18bits.U1RXR = 0x4b;           // Map UART1 RX to RD11
    // Set baud rate to 9600
    // Because of truncation, we get an actual baud rate of circa 9615.38 bps
    // This is a 0.16% error, and UART usually has a 2% tolerance, so we're good
    U1BRG = (unsigned int) ((FCY / (16UL * 9600)) - 1);
    
    U1MODEbits.UARTEN = 1;              // Enable UART1
    U1STAbits.UTXEN = 1;                // Enable U1TX (transimission)
    
    while (1) {
        // Wait for a character to be received
        while (!U1STAbits.URXDA);
        char character = U1RXREG;       // Read RX register

        // Transmit the received character back
        while (U1STAbits.UTXBF);        // Wait if the transmit buffer is full
        U1TXREG = character;            // Write to TX register
    }

    return 0;
}
