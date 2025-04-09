#include <xc.h>
#include <stdlib.h>
#include "general_purpose_lib.h"
#include "timer_lib.h"


void set_digital_mode(void) {
    // Sets all analog pins to digital mode
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0;
}

void leds_init(void) {
    // Set output mode for the LED1/2 pins
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    // Make sure leds are turned off
    LED1 = 0;
    LED2 = 0;
}

void global_interrupt_enable(void) {
    INTCON2bits.GIE = 1;       
}

void algorithm (void) {
    // Simulates an algorithm which takes 7 ms to execute
    tmr_wait_ms(TIMER2, 7);
}

//int Buffer_Init(volatile CircularBuffer* cb, unsigned int size) {
//    // Initializes the buffer structure and allocates memory for the data array.
//    // Returns 0 on success, -1 if memory allocation fails.
//    cb->head = 0;
//    cb->tail = 0;
//    cb->count = 0;
//    cb->size = size;
//    cb->buffer = (volatile char *)malloc(size * sizeof(char));
//    
//    if(cb->buffer == NULL) {
//        return -1;  // Memory allocation error
//    }
//    return 0;
//}
int Buffer_Init(volatile CircularBuffer* cb, volatile char* buf, unsigned int size) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->size = size;
    cb->buffer = buf;   // Use static memory
    return 0;
}

int Buffer_Write(volatile CircularBuffer* cb, char data) {
    // Writes one character into the circular buffer.
    // Returns 0 on success, or -1 if the buffer is full.
    if (cb->count == cb->size) {
        return -1;  // Buffer is full.
    }
    
    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count++;
    return 0;
}

int Buffer_Read(volatile CircularBuffer* cb, char* data) {
    // Reads one character from the circular buffer into the location pointed to by data.
    // Returns 0 on success, or -1 if the buffer is empty.
    if (cb->count == 0) {
        return -1;  // Buffer is empty.
    }
    
    *data = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % cb->size;
    cb->count--;
    return 0;
}