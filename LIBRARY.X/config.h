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
#ifndef CONFIG_H
#define	CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// GENERAL
#define FCY 72000000UL      // Instruction cycle frequency
#define MAX_DELAY 233       // Max int value to not overflow 16-bit timer register

// TIMERS
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3
#define TIMER4 4
#define TIMER5 5
#define TIMER6 6
#define TIMER7 7
#define TIMER8 8
#define TIMER9 9

// UART
#define UART_BAUD_RATE 9600
#define RX_BUFFER_SIZE 16
// Because at 9600 bps we need about 1 ms to receive 1 byte, and considering that ...
// the main loop empties the buffer every 10 ms, in the worst case up to 10 bytes ...
// ... could accumulate. A 16 byte rxBuffer should be sufficient and not overflow
#define TX_BUFFER_SIZE 64
// Because x, y, z and angle_north are signed ints, they can be represented with 6 ...
// ... (5+1 for the sign) characters. Therefore, in the worst case we have:
// - $MAG,x,y,z*\n = 27 bytes every 100 ms (when rate_mag_fb is set to 10 Hz)
// - $YAW,angle*\n = 13 bytes every 200 ms (rate_yaw_fb = 5 Hz)
// - $ERR,1*\n = 8 bytes sporadically
// In total we have 48 bytes to transmit every 200 ms (assuming no more than 1 error ...
// ... message every 200 ms is needed). At 9600 bps, a byte requires about 1 ms, ...
// ... so the 48 bytes should take no more than 50 ms to fully transmit. Therefore, ...
// ... a 64 byte txBuffer should suffice and never overflow, unless we trigger more ...
// ... error or missed bytes messages than we assumed

//SPI
#define CS_ACC LATBbits.LATB3            
#define CS_GYR LATBbits.LATB4
#define CS_MAG LATDbits.LATD6

// LEDS
#define LED1 LATAbits.LATA0
#define LED2 LATGbits.LATG9

// BUTTONS
#define BUTTON1 PORTEbits.RE8
#define BUTTON2 PORTEbits.RE9

// LIGHTS
#define LEDL LATBbits.LATB8 
#define LEDR LATFbits.LATF1
#define LEDBRAKE LATGbits.LATG1
#define LEDFRONT LATAbits.LATA7

// MAGNETOMETER
#define MAG_AVG_SAMPLES 5

// PWM OC
#define PWMA LATDbits.LATD1 
#define PWMB LATDbits.LATD2
#define PWMC LATDbits.LATD3 
#define PWMD LATDbits.LATD4 
#define PWM_FREQ 10000UL
#define PWM_PERIOD_TICKS (FCY/PWM_FREQ)

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

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* CONFIG_H */

