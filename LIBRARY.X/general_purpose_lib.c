#include <xc.h>
#include <stdlib.h>
#include "general_purpose_lib.h"
#include "timer_lib.h"
#include "spi_lib.h"


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


// Initialize the averaging buffer
void MagDataBuffer_Init(MagDataBuffer* mb) {
    mb->index = 0;
    mb->count = 0;
    for (int i = 0; i < MAG_AVG_SAMPLES; i++) {
        mb->x[i] = mb->y[i] = mb->z[i] = 0;
    }
}

// Add a new set of raw magnetometer data
void MagDataBuffer_Write(MagDataBuffer* mb, int x, int y, int z) {
    mb->x[mb->index] = x;
    mb->y[mb->index] = y;
    mb->z[mb->index] = z;
    mb->index = (mb->index + 1) % MAG_AVG_SAMPLES;
    if(mb->count < MAG_AVG_SAMPLES)
        mb->count++;
}

// Compute the average values of the stored samples
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
    unsigned int lsb;
    unsigned int msb;
    
    CS_MAG = 0;
    spi_write(0x42 | 0x80);
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    CS_MAG = 1;
    
    msb = msb << 8;
    
    int result_x = msb | (lsb & 0b11111000);
    result_x = result_x / 8;
    
    CS_MAG = 0;
    spi_write(0x44 | 0x80);
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    CS_MAG = 1;
    
    msb = msb << 8;
    
    int result_y = msb | (lsb & 0b11111000);
    result_y = result_y / 8;
    
    CS_MAG = 0;
    spi_write(0x46 | 0x80);
    lsb = spi_write(0x00);
    msb = spi_write(0x00);
    CS_MAG = 1;
    
    msb = msb << 8;
    
    int result_z = msb | (lsb & 0b11111000);
    result_z = result_z / 8;

    axes_ptr[0] = result_x;
    axes_ptr[1] = result_y;
    axes_ptr[2] = result_z;
}


void mag_read_axes_v2(int* axes_ptr) {
    /**
    * @brief Reads X, Y, Z magnetic field axes using SPI burst read (standard types).
    *
    * @param axes Pointer to an array of 3 ints to store the results
    * (axes[0]=X, axes[1]=Y, axes[2]=Z).
    * @note Relies on 'int' being 16 bits and 'unsigned char' being 8 bits
    * on the target platform (dsPIC33 with XC16).
    * @note Assumes spi_read_address handles multi-byte reads correctly after
    * sending the initial address with the read flag.
    * @note Chip Select (CS) must be handled externally if spi_read_address
    * does not manage it.
    */
    
    unsigned char raw_data[6]; // Buffer for raw data

    // --- Perform Burst Read ---
    // Use the existing spi_read_address function from your spi_lib.h
    CS_MAG = 0; // Assert CS (if not handled by spi_read_address)
    spi_read_address(0x42, raw_data, 6);
    CS_MAG = 1; // De-assert CS (if not handled by spi_read_address)

    // --- Process X-axis --- (Bytes 0=LSB, 1=MSB)
    // LSB bits <7:3> are data <4:0>. MSB bits <7:0> are data <12:5>.
    // Combine: (MSB << 8) | (LSB & 0xF8). Then shift right by 3.
    // Casting MSB to 'int' ensures sign extension before shifting left.
    // The division performs an arithmetic right shift, handling sign.
    axes_ptr[0] = (int)(((int)raw_data[1] << 8) | (raw_data[0] & 0xF8)) / 8;

    // --- Process Y-axis --- (Bytes 2=LSB, 3=MSB)
    // LSB bits <7:3> are data <4:0>. MSB bits <7:0> are data <12:5>.
    // Combine: (MSB << 8) | (LSB & 0xF8). Then shift right by 3.
    axes_ptr[1] = (int)(((int)raw_data[3] << 8) | (raw_data[2] & 0xF8)) / 8;

    // --- Process Z-axis --- (Bytes 4=LSB, 5=MSB)
    // LSB bits <7:1> are data <6:0>. MSB bits <7:0> are data <14:7>.
    // Combine: (MSB << 8) | (LSB & 0xFE). Then shift right by 1.
    axes_ptr[2] = (int)(((int)raw_data[5] << 8) | (raw_data[4] & 0xFE)) / 2;
}

// This function calls mag_read_axes(), and then adds the new readings into the averaging buffer.
void mag_update_readings(MagDataBuffer* mb) {
    int raw_axes[3];
    // Read raw axes
    mag_read_axes_v2(raw_axes);  
    // Update MagDataBuffer pointed to by mb with the new measurements
    MagDataBuffer_Write(mb, raw_axes[0], raw_axes[1], raw_axes[2]);
}