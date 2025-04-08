#include "xc.h"
#include "../LIBRARY.X/uart_lib.h"


int main(void) {
    
    uart_init();
    
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
