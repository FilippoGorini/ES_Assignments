#include <xc.h>
#include "spi_lib.h"


void spi_init() {
    // Configure TRIS (Set up SPI I/O pins)
    TRISAbits.TRISA1 = 1;               // RA1-RPI17  MISO (input)
    TRISFbits.TRISF12 = 0;              // RF12-RP108 SCK (output)
    TRISFbits.TRISF13 = 0;              // RF13-RP109 MOSI (output)
    TRISBbits.TRISB3 = 0;               // CS1 (output)
    TRISBbits.TRISB4 = 0;               // CS2 (output)
    TRISDbits.TRISD6 = 0;               // CS3 (output)

    // Configure Peripheral Pin Select (PPS) mapping
    RPINR20bits.SDI1R = 0b0010001;      // MISO (SDI1) - RPI17
    RPOR12bits.RP109R = 0b000101;       // MOSI (SDO1) - RF13
    RPOR11bits.RP108R = 0b000110;       // SCK1 - RF12
    
    // Configure default value of CS bits
    CS_ACC = 1;                         // Disable accelerometers
    CS_GYR = 1;                         // Disable gyroscope
    CS_MAG = 1;                         // Disable magnetometer
    
    // Configure SPI module to run at FCY/12 = 6 MHz (which is less than the max mag frequency = 7.5 MHz)
    SPI1CON1bits.MSTEN = 1;             // Enable master mode
    SPI1CON1bits.MODE16 = 0;            // 8-bit mode
    SPI1CON1bits.PPRE = 0b10;           // Primary prescaler: 4:1         
    SPI1CON1bits.SPRE = 0b101;          // Secondary prescaler: 3:1
    SPI1CON1bits.CKP = 1;               // Clock idle state = 1
    SPI1CON1bits.CKE = 0;               // Master writes data on falling edge
    // These last two settings correspond to SPI mode 3

    // Enable SPI
    SPI1STATbits.SPIEN = 1;           
}


unsigned int spi_write(unsigned int data) {
    // Write data to the MOSI line
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = data;
    // Read data received on the MISO line
    while (SPI1STATbits.SPIRBF == 0);   //SPIRBF = SPI1 Receive Buffer Full Status bit
    return SPI1BUF;  
}

void spi_write_address(unsigned char address, unsigned char data) {
    // This function takes 1 byte and writes it to the specified address

    address &= 0x7F;                    // Make sure that msb is 0 (write flag)
    spi_write(address);                 // Send register address
    spi_write(data);                    // Send data
}

void spi_read_address(unsigned char address, unsigned char* buf_ptr, int length) {
    // This function reads "length" number of bytes, starting from the specified ...
    // ... address, and puts them in the buffer passed as a pointer to the function
    
    address |= 0x80;                    // Make sure that msb is 1 (read flag)
    spi_write(address);                 // Send register address
    for (int i = 0; i < length; i++) {
        buf_ptr[i] = spi_write(0x00);   // Clock out zeros to read
    }
}