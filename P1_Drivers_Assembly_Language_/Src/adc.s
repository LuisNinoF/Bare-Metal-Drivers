// Registers for ADC Initialization ---> GPIO PIN

.equ 	RCC_BASE,			0x40023800							// RCC: Reset and Clock Control

.equ	AHB1ENR_OFFSET,		0x30
.equ	RCC_AHB1NER,		RCC_BASE + AHB1ENR_OFFSET			// AHB1ENR: AHB1 enable register
.equ	GPIOA_EN,			(1<<0)

.equ	GPIOA_BASE,			0x40020000							// GPIOA: General Purpose Input Output Port A
.equ	GPIOA_MODER_OFFSET,	0x00
.equ	GPIOA_MODER,		GPIOA_BASE + GPIOA_MODER_OFFSET		// MODER: Mode Register
.equ	MODER1_ANLG_SLT,	0xC									// C (base 16) = 1100 (base 2)

// Registers for ADC Initlialization ---> ADC MODULE

.equ	APB2ENR_OFFSET,			0x44
.equ	RCC_APB2NER,			RCC_BASE + APB2ENR_OFFSET		// APB2ENR: APB2 enable register
.equ	ADC1_EN,				(1<<8)

.equ	ADC1_BASE,				0x40012000
.equ	ADC1_CR2_OFFSET,		0x08
.equ	ADC1_CR2,				ADC1_BASE + ADC1_CR2_OFFSET		// CR2: Control register 2

.equ	ADC1_SQR3_OFFSET,		0x34
.equ	ADC1_SQR3,				ADC1_BASE + ADC1_SQR3_OFFSET	// SQR3: Sequence register 3
.equ	SQR3_CNF,				1								// Conversion sequence starts at CH1

.equ	ADC1_SQR1_OFFSET,		0x2C
.equ	ADC1_SQR1,				ADC1_BASE + ADC1_SQR1_OFFSET	// SQR3: Sequence register 1
.equ	SQR1_CNF,				0								// Conversion sequence starts at CH1

.equ	CR2_ADCON,				(1<<0)							// to enable ADC Module


// Registers for ADC Read by starting, waiting and storing conversion

.equ	CR2_SWSTART,			(1<<30)

.equ	ADC1_SR_OFFSET,			0x00
.equ	ADC1_SR,				ADC1_BASE + ADC1_SR_OFFSET		// SR: Status register
.equ	SR_EOC,					(1<<1)

.equ	ADC1_DR_OFFSET,			0x4C
.equ	ADC1_DR,				ADC1_BASE + ADC1_DR_OFFSET		// DR: Data register


// Directives

			.syntax 	unified
			.cpu 		cortex-m4
			.fpu 		softvfp
			.thumb
			.section .text.Reset_Handler
			.globl adc_init
			.globl adc_read


// ADC Initialization

adc_init:

	// ADC Initlialization ---> GPIO PIN

			// 1. Enable clock access to ADC pin's GPIO port (Pin PA1, so Port A)
			ldr r0, =RCC_AHB1NER
			ldr r1, [r0]
			orr r1, #GPIOA_EN
			str r1, [r0]

			// 2. Set ADC PIN (PA1) as analog pin with the mode
			ldr r0, =GPIOA_MODER
			ldr r1, [r0]
			orr r1, #MODER1_ANLG_SLT
			str r1, [r0]

	// ADC Initlialization ---> ADC MODULE

			// 3. Enable clock access to the ADC Module, connected to APB2 bus
			ldr r0, =RCC_APB2NER
			ldr r1, [r0]
			orr r1, #ADC1_EN
			str r1, [r0]

			// 4. Select software trigger to start conversion (reset ADC1_CR2 to 0)
			ldr r0, =ADC1_CR2
			ldr r1, =0x00000000
			str r1, [r0]

			// 5. Set conversion sequence starting channel (where to start)
			ldr r0, =ADC1_SQR3
			ldr r1, =#SQR3_CNF
			str r1, [r0]

			// 6. Set conversion sequence length (in our case only one channel)
			ldr r0, =ADC1_SQR1
			ldr r1, =#SQR1_CNF
			str r1, [r0]

			// 7. Enable ADC Module
			ldr r0, =ADC1_CR2
			ldr r1, [r0]
			orr r1, #CR2_ADCON
			str	r1, [r0]

			bx lr


// ADC Read by starting, waiting and storing conversion

adc_read:
			// 1. Start conversion
			ldr r0, =ADC1_CR2
			ldr r1, [r0]
			orr r1, #CR2_SWSTART
			str	r1, [r0]

			// 2. Wait for conversion to be complete
wait_conversion:
			ldr r0, =ADC1_SR
			ldr r1, [r0]
			and r1, #SR_EOC
			cmp	r1, #0x00
			beq	wait_conversion

			// 3. Read content of ADC data register to get data
			ldr r2, =ADC1_DR
			ldr r0, [r2]					// r0 is the retain register, so it retains a value.
			bx lr


// Stop and end

stop:
			b	stop

			.align
			.end
