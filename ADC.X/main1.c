#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "../LIBRARY.X/config.h"
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"



volatile char tx_buffer_array[TX_BUFFER_SIZE];
volatile CircularBuffer txBuffer;

void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {
    
    IFS0bits.U1TXIF = 0;
    char byte;
    while (!U1STAbits.UTXBF) {
        if (Buffer_Read(&txBuffer, &byte) == 0) {
            U1TXREG = byte;
        } else {
            uart_tx_interrupt_disable();
            // If txBuffer is empty, there are no more characters to be sent for now so ...
            // ... we disable the tx interrupt. It will be reenabled by the uart_send_string ...
            // ... function once new characters are added to the txBuffer
            break;  // Exit the loop to avoid getting stuck in a infinite loop
        }
    }
}


int main(void) {
    global_interrupt_enable();
    
    set_digital_mode();
    
    ANSELBbits.ANSB11 = 1;
    //TRISBbits.TRISB11 = 1;


    AD1CON3bits.ADCS = 8;

    // --- SAMPLING SETUP ---
    AD1CON1bits.ASAM = 0; // manual sampling
    AD1CON3bits.SAMC = 16; // manual sampling time
    AD1CON1bits.AD12B = 1; //12 bits mode
    // --- CONVERSION SETUP ---
    
    Buffer_Init(&txBuffer, tx_buffer_array, TX_BUFFER_SIZE);        // Initialize TX buffer
    uart_init();
        
    
    tmr_setup_period(TIMER2, 10);
    
    AD1CHS0bits.CH0SA = 11; // Select AN11 as input
    AD1CON1bits.ADON = 1;   // Turn on ADC

    while (1) {
        AD1CON1bits.SAMP = 1;
        tmr_wait_ms(TIMER1, 1);  // Wait for sampling
        AD1CON1bits.SAMP = 0;      // Start conversion
        
        LED1 = 1;

        while (!AD1CON1bits.DONE); // Wait until conversion completes

        unsigned int adc_value = ADC1BUF0;
        double v_adc = (adc_value / 4095.0) * 3.3; // assuming Vref+ = 3.3V
        double v_battery = v_adc*3;

        char buffer[32];
        sprintf(buffer, "%.2fV\n", v_battery);
        uart_send_string(&txBuffer, buffer);

        tmr_wait_period(TIMER2);
        
    }
    
    return 0;
}