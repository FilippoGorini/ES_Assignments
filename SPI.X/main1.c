#include "xc.h"

#include "../LIBRARY.X/timer_lib.h"


void spi_init() {
    // Configure TRIS (Set up SPI I/O pins)
    TRISAbits.TRISA1 = 1;               // RA1-RPI17  MISO (input)
    TRISFbits.TRISF12 = 0;              // RF12-RP108 SCK (output)
    TRISFbits.TRISF13 = 0;              // RF13-RP109 MOSI (output)

    // Configure Peripheral Pin Select (PPS) mapping
    RPINR20bits.SDI1R = 0b0010001;      // MISO (SDI1) - RPI17
    RPOR12bits.RP109R = 0b000101;       // MOSI (SDO1) - RF13
    RPOR11bits.RP108R = 0b000110;       // SCK1 - RF12

    // Configure SPI module (before enabling it)
    SPI1CON1bits.MSTEN = 1;             // Enable master mode
    SPI1CON1bits.MODE16 = 0;             // 8-bit mode
    SPI1CON1bits.PPRE = 3;               // 1:1 primary prescaler
    SPI1CON1bits.SPRE = 3;               // 5:1 secondary prescaler

    // Enable SPI (final step)
    SPI1STATbits.SPIEN = 1;             // Enable SPI module
    
}

unsigned int spi_write(unsigned int data) {
    
    //write data
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = data;
        
    //read data
    while (SPI1STATbits.SPIRBF != 1); //SPIRBF = SPI1 Receive Buffer Full Status bit
    return SPI1BUF;  
}


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
    
    
    unsigned int chipID = 0;
    //set the SLEEP mode --> the bit of register before set to 0 -->write
    //write '0' in the register 
    spi_init();
    //ask to magnometer to send me the chipID
    unsigned int reg_mode = 0x4B;
    unsigned int dummy = spi_write (reg_mode & 0xFE);
    dummy = spi_write(1);
    
    tmr_wait_ms(TIMER1, 3); //i have to wait 3ms to set on activeted mode
    
    //set on Active mode
    dummy = spi_write (0x4C & 0xFE);
    dummy = spi_write(0b00);
    
    //read the chipID
    dummy = spi_write (0x40 || 0x01);
    chipID = spi_write (dummy);
    
    //write on the UART
    while (U1STAbits.UTXBF);        // Wait if the transmit buffer is full
    U1TXREG = chipID;
    
    
    return 0;
}
