// APA102 SPI bit-bang PRU code for Bela Gem / PocketBeagle2
//
// Clocks out APA102 frames using two GPIO pins (clock + data)
// via the PRU on bela gem's SoC.
//
// PRU-side pin numbers may differ from Linux-side GPIO numbers.
// Adjust GPIO_CLOCK_PIN and GPIO_DATA_PIN below to match the
// bit positions in the PRU GPIO register for the PB2.

#include "../../pru/board_specific.h"

#define GPIO_BASE 0x00600038

#define GPIO_CLOCK_PIN 17 // corresponds to GPIO pin 49
#define GPIO_DATA_PIN 18  // corresponds to GPIO pin 50

#define BIT_DELAY_LOW 70
#define BIT_DELAY_HIGH 90

.origin 0
.entrypoint START

.macro NOP
	MOV r24, r24
.endm

.macro DELAY
.mparam cycles, label
    NOP
    MOV r7, cycles-1
label:
    SUB r7, r7, 1
    QBNE label, r7, 0
.endm

.macro CLOCK_LOW
    SBBO r22, r20, 0, 4
.endm

.macro CLOCK_HIGH
    SBBO r22, r21, 0, 4
.endm

.macro DATA_LOW
    SBBO r23, r20, 0, 4
.endm

.macro DATA_HIGH
    SBBO r23, r21, 0, 4
.endm


START:
	// GPIO setup
	MOV r20, GPIO_BASE + GPIO_CLEARDATAOUT
	MOV r21, GPIO_BASE + GPIO_SETDATAOUT
	MOV r22, (0 | (1 << GPIO_CLOCK_PIN))
	MOV r23, (0 | (1 << GPIO_DATA_PIN))
	
    /** Enable OCP master port */
    LBCO r0, C4, 4, 4
    CLR r0, r0, 4
    SBCO r0, C4, 4, 4

    /** Indicate PRU has started */
    MOV r2, #0x1
    SBCO r2, C24, 12, 4
    

_LOOP:

    /** Wait for start command */
    LBCO r0, C24, 0, 12            // Load data_addr, data_len, start command
    QBEQ _LOOP, r2, #0             // Loop until start command is non-zero
    
    /** Cache data address and length */
    MOV r18, r0
	MOV r19, r1

    /** Clear start command */
    MOV r3, 0
    SBCO r3, C24, 8, 4

    /** Check for exit command */
    QBEQ EXIT, r2, #0xFF
    
START_FRAME:
    DATA_LOW
    QBA WAIT_FULL_FRAME

END_FRAME:
    DATA_HIGH

WAIT_FULL_FRAME:
    // 32 clock cycles for start/end frame
    MOV r8, 32                  // Initialize our loop counter to 32
    
frame_clock_loop:
    CLOCK_LOW
    DELAY BIT_DELAY_LOW, led_frame_lo
    CLOCK_HIGH
    DELAY BIT_DELAY_HIGH, led_frame_hi
    
    SUB r8, r8, 1               // Decrement counter
    QBNE frame_clock_loop, r8, 0 // Jump back if counter is not 0
    
    QBEQ FRAME_COMPLETE, r26, 1
    
WORD_LOOP:
    MOV r6, 8
    LBCO r17, C24, r18, 1

BIT_LOOP:
    SUB r6, r6, 1
    QBBS BIT_ONE, r17, r6
    QBA BIT_ZERO

BIT_ONE:
    DATA_HIGH
    QBA BIT_END

BIT_ZERO:
    DATA_LOW
    QBA BIT_END

BIT_END:
    CLOCK_LOW
    DELAY BIT_DELAY_LOW, led_data_clock_low
    CLOCK_HIGH
    DELAY BIT_DELAY_HIGH, led_data_clock_high

    QBNE BIT_LOOP, r6, 0

    /** Move to the next byte */
    ADD r18, r18, 1
    SUB r19, r19, 1
    QBNE WORD_LOOP, r19, 0

ALL_LEDS_PROCESSED:

	MOV r26, 1
    QBA END_FRAME

FRAME_COMPLETE:

	MOV r26, 0

    /** Notify completion */
    MOV r2, #0x1
    SBCO r2, C24, 12, 4

    QBA _LOOP

EXIT:
    /** Notify stop and halt */
    MOV r2, #0xFF
    SBCO r2, C24, 12, 4
    MOV R31.b0, 19 + 16
    HALT
