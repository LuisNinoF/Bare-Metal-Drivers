// Register address for USART Initialization ---> GPIO PIN

.equ 	RCC_BASE,			0x40023800						// RCC: Reset and Clock Control
.equ	AHB1ENR_OFFSET,		0x30
.equ	RCC_AHB1ENR,		RCC_BASE + AHB1ENR_OFFSET		// AHB1ENR: AHB1 enable register
.equ	GPIOA_EN,			(1<<0)

.equ	GPIOA_BASE,			0x40020000						// GPIOA: General Purpose Input Output Port A
.equ	GPIOA_MODER_OFFSET,	0x00
.equ	GPIOA_MODER,		GPIOA_BASE + GPIOA_MODER_OFFSET	// MODER: Mode Register
.equ	MODER_AF_SELECT,	0x20							// Select alternative function as mode

.equ	GPIOx_AFRL_OFFSET,	0x20
.equ	GPIOx_AFRL,			GPIOA_BASE + GPIOx_AFRL_OFFSET	// APB1ENR: APB1 enable register (Set it to AF7)
.equ	AFRL2_AF7,			0x0700							// Select alternative function 7

//  Register address for USART Initialization ---> UART2 Module

.equ	APB1ENR_OFFSET,		0x40
.equ	RCC_APB1ENR,		RCC_BASE + APB1ENR_OFFSET		// APB1ENR: APB1 enable register
.equ	USART2_EN,			(1<<17)

.equ	USART2_BASE,		0x40004400
.equ	USART_BRR_OFFSET,	0x08
.equ	USART_BRR,			USART2_BASE + USART_BRR_OFFSET	// BRR: Baud rate register
.equ	BRR_CNF,			0x683							// 9600 Baudrate @16 MHz

.equ	USART_CR1_OFFSET,	0x0C
.equ	USART_CR1,			USART2_BASE + USART_CR1_OFFSET	// CR1: Control register 1 (Set for transmitter)
.equ	USART_CR1_CNF,		0X008

.equ	USART_CR2_OFFSET,	0x10
.equ	USART_CR2,			USART2_BASE + USART_CR2_OFFSET	// CR2: Control register 2
.equ	USART_CR2_CNF,		0X000

.equ	USART_CR3_OFFSET,	0x14
.equ	USART_CR3,			USART2_BASE + USART_CR3_OFFSET	// CR3: Control register 3
.equ	USART_CR3_CNF,		0X000

.equ	USART_CR1_EN,		(1<<13)							// CR1: Control register 1 (to enable USART Module)

// Registers for USART data transmission

.equ	USART_SR_OFFSET,	0x00
.equ	USART_SR,			USART2_BASE + USART_SR_OFFSET	// SR: Status Register
.equ	USART_SR_TXE,		(1<<7)

.equ	USART_DR_OFFSET,	0x04
.equ	USART_DR,			USART2_BASE + USART_DR_OFFSET	// DR: DataRegister


// Directives
			.syntax 	unified
			.cpu 		cortex-m4
			.fpu 		softvfp
			.thumb
			.section .text.Reset_Handler
			.globl uart_init
			.globl uart_transmit


// USART Initialization

uart_init:

	// USART Initialization ---> GPIO PIN

			// 1. Enable clock access to UART GPIO pins (GPIOA because PA3 for USART2_TX and PA2 for USART2_TX)
			ldr r0, =RCC_AHB1ENR
			ldr r1, [r0]
			orr r1, #GPIOA_EN
			str r1, [r0]

			// 2. Set UART gpio pin mode to alternate function mode. => Set PA to alternate function
			ldr r0, =GPIOA_MODER
			ldr r1, [r0]
			bic	r1, #0x30
			str r1, [r0]

			ldr r0, =GPIOA_MODER
			ldr r1, [r0]
			orr r1, #MODER_AF_SELECT
			str r1, [r0]

			// 3. Set UART gpio pin alternate function type to UART
			ldr r0, =GPIOx_AFRL
			ldr r1, [r0]
			bic r1, #0x0F00
			str r1, [r0]

			ldr r0, =GPIOx_AFRL
			ldr r1, [r0]
			orr r1, #AFRL2_AF7
			str r1, [r0]


	//  USART Initialization ---> UART2 Module

			// 4. Enable clock access to UART2 module
			ldr r0, =RCC_APB1ENR
			ldr r1, [r0]
			orr r1, #USART2_EN
			str r1, [r0]

			// 5. Set UART baudrate (speed of communication)
			ldr r0, =USART_BRR
			mov r1, #BRR_CNF
			str r1, [r0]

			// 6. Configure control register 1
			ldr r0, =USART_CR1
			mov r1, #USART_CR1_CNF
			str r1, [r0]

			// 7. Configure control register 2
			ldr r0, =USART_CR2
			mov r1, #USART_CR2_CNF
			str r1, [r0]

			// 8. Configure control register 3
			ldr r0, =USART_CR3
			mov r1, #USART_CR3_CNF
			str r1, [r0]

			// 9. Enable UART module
			ldr r0, =USART_CR1
			ldr r1, [r0]
			orr r1, #USART_CR1_EN
			str r1, [r0]

			bx lr


// USART data transmission

uart_transmit:

			// 1. Make sure UART transmit fifo is not full, by checking the USART Status Register
			ldr r1, =USART_SR

			// 2. Wait until data is transfered, so that the register is not full
wait_until_transfered:
			ldr r2, [r1]
			and r2, #USART_SR_TXE
			cmp	r2, #0x00
			beq	wait_until_transfered

			// 3. Write data into uart data register
			mov r1, r0
			ldr r2, =USART_DR
			str r1, [r2]
			bx lr

// End

			.align
			.end
