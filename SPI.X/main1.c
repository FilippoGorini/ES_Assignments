#include "xc.h"

void spi_init(void) {
    // Configure TRIS (Set up SPI I/O pins)
    TRISAbits.TRISA1 = 1;               // RA1-RPI17  MISO (input)
    TRISFbits.TRISF12 = 0;              // RF12-RP108 SCK (output)
    TRISFbits.TRISF13 = 0;              // RF13-RP109 MOSI (output)

    // Configure Peripheral Pin Select (PPS) mapping
    RPINR20bits.SDI1R = 0b0010001;      // MISO (SDI1) - RPI17
    RPOR12bits.RP109R = 0b000101;       // MOSI (SDO1) - RF13
    RPOR11bits.RP108R = 0b000110;       // SCK1 - RF12

    // Configure SPI module (before enabling it)
    SPI1CONbits.MSTEN = 1;              // Enable master mode
    SPI1CONbits.MODE16 = 0;             // 8-bit mode
    SPI1CONbits.PPRE = 3;               // 1:1 primary prescaler
    SPI1CONbits.SPRE = 3;               // 5:1 secondary prescaler

    // Enable SPI (final step)
    SPI1STATbits.SPIEN = 1;             // Enable SPI module
}


unsigned int spi_write(unsigned int data) {
    
}

int main(void) {
    return 0;
}
