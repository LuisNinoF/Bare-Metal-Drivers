// Register Addresses for GPIO output

.equ 	RCC_BASE,			0x40023800								// RCC: Reset and Clock Control
.equ	AHB1ENR_OFFSET,		0x30
.equ	RCC_AHB1NER,		RCC_BASE + AHB1ENR_OFFSET				// AHB1ENR: AHB1 enable register
.equ	GPIOA_EN,			(1<<0)

.equ	GPIOA_BASE,			0x40020000								// GPIOA: General Purpose Input Output Port A
.equ	GPIOA_MODER_OFFSET,	0x00
.equ	GPIOA_MODER,		GPIOA_BASE + GPIOA_MODER_OFFSET			// MODER: Mode Register
.equ	MODER5_OUT,			(1<<10)

.equ	GPIOA_BSRR_OFFSET,	0x18
.equ	GPIOA_BSRR,			GPIOA_BASE + GPIOA_BSRR_OFFSET			// BSRR: Bit Set/Reset Register
.equ	BSRR_5_SET,			(1<<5)
.equ	BSRR_5_RESET,		(1<<21)


// Directives
			.syntax 	unified
			.cpu 		cortex-m4
			.fpu 		softvfp
			.thumb
			.section .text.Reset_Handler
			.globl led_init
			.globl led_on
			.globl led_off

// LED as GPIO Output Initialization
led_init:
			// 1. Enable clock access to GPIOA
			ldr r0, =RCC_AHB1NER
			ldr r1, [r0]
			orr r1, #GPIOA_EN
			str r1, [r0]

			// 2. Set PA5 as output
			ldr r0, =GPIOA_MODER
			ldr r1, [r0]
			orr r1, #MODER5_OUT
			str r1, [r0]

			bx lr

// Turn LED on using the Bit Set/Reset Register
led_on:
			// 1.
			mov r1, #0
			ldr	r2, =GPIOA_BSRR
			mov	r1, #BSRR_5_SET
			str	r1, [r2]
			bx	lr

// Turn LED off susing the Bit Set/Reset Register
led_off:
			mov r1, #0
			ldr	r2, =GPIOA_BSRR
			mov	r1, #BSRR_5_RESET
			str	r1, [r2]
			bx	lr

// Stop and end
stop:
			b	stop

			.align
			.end
