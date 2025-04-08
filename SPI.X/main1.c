#include <xc.h>
#include <stdio.h>
#include "string.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/uart_lib.h"
#include "../LIBRARY.X/spi_lib.h"


int main(void) {
    
    set_digital_mode();
    uart_init();
    
    unsigned int chipID = 0;
    //set the SLEEP mode --> the bit of register before set to 0 -->write
    //write '0' in the register 
    spi_init();
    //ask to magnometer to send me the chipID
    CS_MAG = 0;
    unsigned int dummy = spi_write(0x4B);
    dummy = spi_write(0x01);
    CS_MAG = 1;
    
    tmr_wait_ms(TIMER1, 3); //i have to wait 3ms to set on activeted mode
    
    //set on Active mode
    CS_MAG = 0;
    dummy = spi_write(0x4C);
    dummy = spi_write(0b00);
    CS_MAG = 1;

    //read the chipID
    CS_MAG = 0;
    dummy = spi_write(0x40 | 0x80);
    chipID = spi_write(0x00);
    CS_MAG = 1;
    
    char buffer[6]; 
    sprintf(buffer, "%u", chipID);

    for(int i = 0; buffer[i] != '\0'; i++) {
        while (U1STAbits.UTXBF);  // wait if transmit buffer is full
        U1TXREG = buffer[i];      // send char
    }

    while(1);
    
    return 0;
}
