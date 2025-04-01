#include "xc.h"
#include "timer.h"
#include "ring_buffer.h"
#include <string.h>

#define FCY 72000000UL
#define BAUD_RATE 9600
#define LED1 LATAbits.LATA0
#define LED2 LATGbits.LATG9




int received_chars = 0;
char* data;
struct Instruction command;
int ld2_enabled = 1;  // Variable to enable/disable LD2 blinking


struct CircularBuffer* cb;
Buffer_Init(cb);


void algorithm() {
    tmr_wait_ms(TIMER2, 7);  // Waste 7 ms to simulate algorithm
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;  // Clear Timer3 Interrupt Flag
    if (ld2_enabled) {
        LED2 = !LED2;  // Toggle LED2 only if enabled
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    char received_char = U1RXREG;  // Read received character
    IFS0bits.U1RXIF = 0;  // Clear interrupt flag
    received_chars++; // Increment received character counter

    // Store character in circular buffer
    if (Buffer_Write(cb, received_char) == -1){
        //do something
        parser_uart(cb, command);
    }
}

int parser_uart(CircularBuffer* cb, Instruction* command){
    if (Buffer_Read(cb, data) != -1){
        if (data == 'L'){
            //updating command
            command->data[0] = data; 
            return parser_uart(cb);
        }
        if (data == 'D'){
                //updating command
                command->data[1] = data; 
                return parser_uart(cb);
        }
        if (data == '1'){           
            memset(command->data, '0', 3);
            return 1;
        }
        if (data == '2'){
            memset(command->data, '0', 3);
            return 2;
        }
        else {
            memset(command->data, '0', 3);
            return -1;
        }
    }
    else return 0;
}



int main(void) {
    int ret;
    
    TRISAbits.TRISA0 = 0;  // Set RA0 as output (LED1)
    TRISGbits.TRISG9 = 0;  // Set RG9 as output (LED2)
    
    // SETUP INTERRUPTS
    INTCON2bits.GIE = 1;  // Enable global interrupt
    IEC0bits.T3IE = 1;  // Enable Timer3 interrupt
    IFS0bits.U1RXIF = 0;  // Clear UART1 RX interrupt flag
    IEC0bits.U1RXIE = 1;  // Enable UART1 RX interrupt
    
   
    // SETUP TIMERS
    tmr_setup_period(TIMER1, 10);  // Set TIMER1 to 100 Hz
    tmr_setup_period(TIMER3, 200);  // Set TIMER3 to blink LED2 at 2.5Hz
    
    // SETUP UART
    RPOR0bits.RP64R = 0x01;  // Map UART1 TX to RD0
    RPINR18bits.U1RXR = 0x4B;  // Map UART1 RX to RD11
    
    // Set baud rate to 9600
    U1BRG = (unsigned int)((FCY / (16UL * BAUD_RATE)) - 1);
    U1MODEbits.UARTEN = 1;  // Enable UART1
    U1STAbits.UTXEN = 1;  // Enable U1TX (transmission)
    U1STAbits.URXISEL = 0;  // Set U1RX interrupt to be triggered when a new byte is received
    
    while (1) {
        algorithm();
        int tag = parser_uart(cb, command);
        switch (tag){
            case 1:
                LED1 = !LED1;
            case 2:
                ld2_enabled = !ld2_enabled;
        }
        ret = tmr_wait_period(TIMER1);
    }
    
    return 0;
}