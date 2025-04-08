#include "xc.h"
#include "spi_lib.h"


void spi_init() {
    // Configure TRIS (Set up SPI I/O pins)
    TRISAbits.TRISA1 = 1;               // RA1-RPI17  MISO (input)
    TRISFbits.TRISF12 = 0;              // RF12-RP108 SCK (output)
    TRISFbits.TRISF13 = 0;              // RF13-RP109 MOSI (output)
    // Set CS bits as outputs
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISDbits.TRISD6 = 0;

    // Configure Peripheral Pin Select (PPS) mapping
    RPINR20bits.SDI1R = 0b0010001;      // MISO (SDI1) - RPI17
    RPOR12bits.RP109R = 0b000101;       // MOSI (SDO1) - RF13
    RPOR11bits.RP108R = 0b000110;       // SCK1 - RF12
    
    // Configure CS bits
    CS_ACC = 1;                         // Disable accelerometers
    CS_GYR = 1;                         // Disable gyroscope
    CS_MAG = 1;                         // Disable magnetometer
    
    // Configure SPI module (before enabling it)
    SPI1CON1bits.MSTEN = 1;             // Enable master mode
    SPI1CON1bits.MODE16 = 0;             // 8-bit mode
    SPI1CON1bits.PPRE = 2;             
    SPI1CON1bits.SPRE = 5;              
    SPI1CON1bits.CKP = 1; 

    // Enable SPI
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