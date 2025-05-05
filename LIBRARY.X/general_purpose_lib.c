#include <xc.h>
#include <stdlib.h>
#include "general_purpose_lib.h"
#include "timer_lib.h"
#include "spi_lib.h"
#include "pwm_lib.h"


int Buffer_Init(volatile CircularBuffer* cb, volatile char* buf_ptr, unsigned int size) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->size = size;
    cb->buffer = buf_ptr;  
    return 0;
}

int Buffer_Write(volatile CircularBuffer* cb, char data) {
    // Writes one character into the circular buffer.
    // Returns 0 on success, or -1 if the buffer is full.
    if (cb->count == cb->size) {
        return -1;                  // Buffer is full.
    }
    
    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count++;
    return 0;
}

int Buffer_Read(volatile CircularBuffer* cb, char* data_ptr) {
    // Reads one character from the circular buffer into the location pointed to by data.
    // Returns 0 on success, or -1 if the buffer is empty.
    if (cb->count == 0) {
        return -1;                  // Buffer is empty.
    }
    
    *data_ptr = cb->buffer[cb->head];
    cb->head = (cb->head + 1) % cb->size;
    cb->count--;
    return 0;
}

void MagDataBuffer_Init(MagDataBuffer* mb) {
    mb->index = 0;
    mb->count = 0;
    for (int i = 0; i < MAG_AVG_SAMPLES; i++) {
        mb->x[i] = mb->y[i] = mb->z[i] = 0;
    }
}

void MagDataBuffer_Write(MagDataBuffer* mb, int x, int y, int z) {
    mb->x[mb->index] = x;
    mb->y[mb->index] = y;
    mb->z[mb->index] = z;
    mb->index = (mb->index + 1) % MAG_AVG_SAMPLES;
    if(mb->count < MAG_AVG_SAMPLES)
        mb->count++;
}

void MagDataBuffer_Average(MagDataBuffer* mb, int* avg_x, int* avg_y, int* avg_z) {
    long sum_x = 0, sum_y = 0, sum_z = 0;
    for (int i = 0; i < mb->count; i++) {
        sum_x += mb->x[i];
        sum_y += mb->y[i];
        sum_z += mb->z[i];
    }
    if(mb->count > 0) {
        *avg_x = (int)(sum_x / mb->count);
        *avg_y = (int)(sum_y / mb->count);
        *avg_z = (int)(sum_z / mb->count);
    } else {
        *avg_x = *avg_y = *avg_z = 0;
    }
}

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

void lights_init(void) {
    // Set output mode
    TRISBbits.TRISB8 = 0;
    TRISFbits.TRISF1 = 0;
    TRISGbits.TRISG1 = 0;
    TRISAbits.TRISA7 = 0;
    // Make sure leds are turned off
    LEDL = 0;
    LEDR = 0;
    LEDBRAKE = 0;
    LEDFRONT = 0;
}

void global_interrupt_enable(void) {
    INTCON2bits.GIE = 1;       
}

void algorithm (void) {
    // Simulates an algorithm which takes 7 ms to execute
        tmr_wait_ms(TIMER2, 7);
}

void mag_sus2act(void) {
    // This function switches the magnetometer from suspended to active mode
    CS_MAG = 0;
    spi_write_address(0x4B, 0x01);      // Switch from suspended to sleep mode
    CS_MAG = 1;
    
    tmr_wait_ms(TIMER3, 3);             // Wait 3 ms for the switch to take place
    
    CS_MAG = 0;
    spi_write_address(0x4C, 0b00110000);// Switch from sleep to active mode and set 25 Hz data rate
    CS_MAG = 1;
}

void mag_read_axes(int* axes_ptr) {
    
    unsigned char raw_data[6]; // Buffer for raw data 
    // Read raw data
    CS_MAG = 0; 
    spi_read_address(0x42, raw_data, 6);
    CS_MAG = 1; 

    axes_ptr[0] = (int)(((int)raw_data[1] << 8) | (raw_data[0] & 0b11111000)) / 8;
    axes_ptr[1] = (int)(((int)raw_data[3] << 8) | (raw_data[2] & 0b11111000)) / 8;
    axes_ptr[2] = (int)(((int)raw_data[5] << 8) | (raw_data[4] & 0b11111110)) / 2;
}

void mag_update_readings(MagDataBuffer* mb) {
    // This function calls mag_read_axes(), and then adds the new readings into the averaging buffer.
    int raw_axes[3];
    // Read raw axes
    mag_read_axes(raw_axes);  
    // Update MagDataBuffer pointed to by mb with the new measurements
    MagDataBuffer_Write(mb, raw_axes[0], raw_axes[1], raw_axes[2]);
}

void move_forward(void) {
    oc1_set_duty(0);
    oc2_set_duty(100);
    oc3_set_duty(0);
    oc4_set_duty(100);
}

void move_backward(void) {
    oc1_set_duty(100);
    oc2_set_duty(0);
    oc3_set_duty(100);
    oc4_set_duty(0);
}

void rotate_right(void) {
    oc1_set_duty(0);
    oc2_set_duty(100);
    oc3_set_duty(100);
    oc4_set_duty(0);
}

void rotate_left(void) {
    oc1_set_duty(100);
    oc2_set_duty(0);
    oc3_set_duty(0);
    oc4_set_duty(100);
}

void turn_right_forward(void) {
    oc1_set_duty(0);
    oc2_set_duty(100);
    oc3_set_duty(0);
    oc4_set_duty(70);
}

void turn_left_forward(void) {
    oc1_set_duty(0);
    oc2_set_duty(70);
    oc3_set_duty(0);
    oc4_set_duty(100);
}

void turn_right_backward(void) {
    oc1_set_duty(100);
    oc2_set_duty(0);
    oc3_set_duty(70);
    oc4_set_duty(0);
}

void turn_left_backward(void) {
    oc1_set_duty(70);
    oc2_set_duty(0);
    oc3_set_duty(100);
    oc4_set_duty(0);
}