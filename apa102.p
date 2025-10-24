/** Mappings of the GPIO devices */
#define GPIO0 0x44E07000          // Base address of GPIO0
#define GPIO_CLEARDATAOUT 0x190   // Offset for clearing GPIO output data
#define GPIO_SETDATAOUT   0x194   // Offset for setting GPIO output data

#define GPIO_BASE GPIO0           // Use GPIO0 as the base GPIO

#define GPIO_UART1 30
#define GPIO_UART2 31

#define BIT_DELAY 30 // Determined empirically, depending on the number of LEDs. Usually about 20-50

.macro NOP
	MOV r24, r24
.endm

/** Macros for delays */
.macro DELAY
.mparam cycles, label
    NOP
    MOV r7, cycles-1
label:
    SUB r7, r7, 1
    QBNE label, r7, 0
.endm

.macro UART1_LOW
    SBBO r22, r20, 0, 4
.endm

.macro UART1_HIGH
    SBBO r22, r21, 0, 4
.endm

.macro UART2_LOW
    SBBO r23, r20, 0, 4
.endm

.macro UART2_HIGH
    SBBO r23, r21, 0, 4
.endm


START:
	// GPIO setup
	MOV r20, GPIO_BASE | GPIO_CLEARDATAOUT
	MOV r21, GPIO_BASE | GPIO_SETDATAOUT
	MOV r22, (0 | (1 << GPIO_UART1))
	MOV r23, (0 | (1 << GPIO_UART2))
	
    /** Enable OCP master port */
    LBCO r0, C4, 4, 4              // Load SYSCFG register
    CLR r0, r0, 4                  // Clear bit 4 to enable OCP master port
    SBCO r0, C4, 4, 4              // Write back SYSCFG register

    /** Indicate PRU has started */
    MOV r2, #0x1
    SBCO r2, C24, 12, 4            // Notify main program PRU is ready
    

_LOOP:

    /** Wait for start command */
    LBCO r0, C24, 0, 12            // Load data_addr, data_len, start command
    QBEQ _LOOP, r2, #0             // Loop until start command is non-zero
    
    /** Cache data address and length */
    MOV r18, r0
	MOV r19, r1

    /** Clear start command */
    MOV r3, 0
    SBCO r3, C24, 8, 4             // Clear start command in DRAM

    /** Check for exit command */
    QBEQ EXIT, r2, #0xFF           // Exit if start command is 0xFF
    
START_FRAME:
    UART2_LOW
    QBA WAIT_FULL_FRAME

END_FRAME:
    UART2_HIGH

WAIT_FULL_FRAME:
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_1
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_1
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_2
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_2
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_3
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_3
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_4
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_4
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_5
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_5
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_6
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_6
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_7
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_7
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_8
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_8
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_9
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_9
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_10
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_10
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_11
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_11
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_12
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_12
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_13
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_13
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_14
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_14
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_15
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_15
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_16
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_16
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_17
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_17
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_18
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_18
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_19
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_19
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_20
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_20
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_21
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_21
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_22
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_22
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_23
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_23
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_24
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_24
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_25
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_25
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_26
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_26
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_27
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_27
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_28
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_28
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_29
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_29
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_30
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_30
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_31
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_31
    UART1_LOW
    DELAY BIT_DELAY, led_start_lo_32
    UART1_HIGH
    DELAY BIT_DELAY, led_start_hi_32
    
    QBEQ FRAME_COMPLETE, r26, 1
    
WORD_LOOP:
    MOV r6, 8
    LBCO r17, C24, r18, 1

BIT_LOOP:
    SUB r6, r6, 1
    QBBS BIT_ONE, r17, r6          // Branch to BIT_ONE if current bit is 1
    QBA BIT_ZERO                   // Otherwise, branch to BIT_ZERO

BIT_ONE:
    UART2_HIGH
    QBA BIT_END

BIT_ZERO:
    UART2_LOW
    QBA BIT_END

BIT_END:
    UART1_LOW
    DELAY BIT_DELAY, led_data_clock_low
    UART1_HIGH
    DELAY BIT_DELAY, led_data_clock_high

    QBNE BIT_LOOP, r6, 0

    /** Move to the next byte */
    ADD r18, r18, 1
    SUB r19, r19, 1
    QBNE WORD_LOOP, r19, 0          // Loop until all LEDs are processed

ALL_LEDS_PROCESSED:

	MOV r26, 1
    QBA END_FRAME

FRAME_COMPLETE:

	MOV r26, 0

    /** Notify completion */
    MOV r2, #0x1
    SBCO r2, C24, 12, 4            // Indicate completion to main program

    QBA _LOOP                      // Go back to wait for next command

EXIT:
    /** Notify stop and halt */
    MOV r2, #0xFF
    SBCO r2, C24, 12, 4            // Signal PRU is stopping
    MOV R31.b0, 19 + 16            // Send interrupt to host
    HALT                           // Halt the PRU