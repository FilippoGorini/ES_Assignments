/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef GENERAL_PURPOSE_LIB_H
#define	GENERAL_PURPOSE_LIB_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>
#include <stdlib.h>
#include "general_purpose_lib.h"
#include "timer_lib.h"
#include "spi_lib.h"

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations
typedef struct {
    volatile unsigned int head;    // index for reading
    volatile unsigned int tail;    // index for writing
    volatile unsigned int count;   // number of bytes currently stored
    unsigned int size;             // total capacity of the buffer
    volatile char *buffer;         // pointer to the actual data array
} volatile CircularBuffer;

typedef struct {
    int x[MAG_AVG_SAMPLES];
    int y[MAG_AVG_SAMPLES];
    int z[MAG_AVG_SAMPLES];
    int index;                      // Next insertion index
    int count;                      // Number of samples stored (max MAG_AVG_SAMPLES)
} MagDataBuffer;

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

int Buffer_Init(volatile CircularBuffer* cb, volatile char* buf_ptr, unsigned int size);
int Buffer_Write(volatile CircularBuffer* cb, char data);
int Buffer_Read(volatile CircularBuffer* cb, char* data_ptr);
void MagDataBuffer_Init(MagDataBuffer* mb);
void MagDataBuffer_Write(MagDataBuffer* mb, int x, int y, int z);
void MagDataBuffer_Average(MagDataBuffer* mb, int* avg_x, int* avg_y, int* avg_z);
void set_digital_mode(void);
void leds_init(void);
void lights_init(void);
void global_interrupt_enable(void);
void algorithm(void);
void mag_sus2act(void);
void mag_read_axes(int* axes_ptr);
void mag_read_axes_v2(int* axes_ptr);
void mag_update_readings(MagDataBuffer* mb);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* GENERAL_PURPOSE_LIB_H */

