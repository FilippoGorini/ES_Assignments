/*
 * File:   ring_buffer.c
 * Author: smuss
 *
 * Created on 24 March 2025, 12:08
 */


#include "ring_buffer.h"
#include "xc.h"



void Buffer_Init_0 (CircularBuffer_0* cb){
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

int Buffer_Write_0 (CircularBuffer_0* cb, char data){
    if (cb->count == BUFFER_SIZE){
        return -1;
    }
    
    cb->buffer[cb->tail] = data;
    cb->tail = (cb->tail+1) %   BUFFER_SIZE;
    cb->count++;
    return 0;
}

int Buffer_Read_0 (CircularBuffer_0* cb, char* data){
    if (cb->count == 0){
        return -1;
    }
    
    *data = cb->buffer[cb->head];
    cb->head = (cb->head +1) % BUFFER_SIZE;
    cb->count--;
    return 0;
}


