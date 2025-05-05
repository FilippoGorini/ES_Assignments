#include <xc.h>
#include <stdio.h>
#include "../LIBRARY.X/config.h"
#include "../LIBRARY.X/general_purpose_lib.h"
#include "../LIBRARY.X/timer_lib.h"
#include "../LIBRARY.X/uart_lib.h"


int main(void) {
    ANSELBbits.ANSB11 = 1;

    TRISBbits.TRISB11;


    AD1CON3bits.ADCS = 8;

    // --- SAMPLING SETUP ---
    AD1CON1bits.ASAM = 0; // manual sampling
    AD1CON3bits.SAMC = 16; // manual sampling time
    // --- CONVERSION SETUP ---
    
    uart_init();
    
    int battery = 0;
    
    char buffer[64];
    
    
    
    while(1){
    
        AD1CON1bits.ADON = 1;
        tmr_wait_ms(TIMER1, 0.1);
        AD1CON1bits.SAMP = 0;

        //while(!AD1CON1bits.DONE);

        battery = ADC1BUF0;
        
        sprintf(buffer, "%d", battery);
        
        //while (U1STAbits.UTXBF);
        U1TXREG = 'c';     
        

    }
    
    return 0;
}