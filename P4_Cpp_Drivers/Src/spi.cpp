/*NOTE: SPI clock access and gpio pins written fro SPI1
 *Code could be expanded to any SPI (to do)*/

#include "spi.hpp"

/*Define Macros*/
#define SPI1EN 		(1U<<12)
#define GPIOAEN		(1U<<0)

#define SR_TXE		(1U<<1)
#define SR_BSY		(1U<<7)
#define SR_RXNE		(1U<<0)

/*Pinout mapping
 *PA5 (D13) -> CLK
 *PA6 (D12) -> MISO
 *PA7 (D11) -> MOSI
 *PA9 (D8) -> Slave Select*/


/*Constructor to initialize SPI
 *args: SPI peripheral, GPIO port for chip select (cs) and GPIO pin for chip select (cs)*/
SPI::SPI(SPI_TypeDef * spi, GPIO_TypeDef * gpioPort, uint8_t csPin): spi(spi), gpioPort(gpioPort), csPin(csPin){
	configureGPIO();
	configureSPI();
}


void SPI::configureGPIO(){
	// 2. Enable clock access to GPIOA as well, because the pins are in port A, which is in AHB1 bus
	RCC->AHB1ENR |= GPIOAEN;

	// 3. Configure the mode of the pins in the mode register
	// PA5, PA6, PA7 to alternate function to 10
	// PA9 as output pin to 01.
	gpioPort->MODER &=~ (1U<<10);
	gpioPort->MODER |= (1U<<11);

	gpioPort->MODER &=~ (1U<<12);
	gpioPort->MODER |= (1U<<13);

	gpioPort->MODER &=~ (1U<<14);
	gpioPort->MODER |= (1U<<15);

	// Set csPin as output pin
	gpioPort->MODER |=  (1U<<(csPin * 2));
	gpioPort->MODER &=~ (1U<<(csPin * 2 + 1));

	// 4. Configure alternate function type SPI1 for pins PA5, PA6, PA7
	// by setting AF05 in the alternate function register (AFR)
	// Set corresponding bits to 0101 (AF05)
	gpioPort->AFR[0] |= (1U<<20);
	gpioPort->AFR[0] &=~ (1U<<21);
	gpioPort->AFR[0] |= (1U<<22);
	gpioPort->AFR[0] &=~ (1U<<23);

	gpioPort->AFR[0] |= (1U<<24);
	gpioPort->AFR[0] &=~ (1U<<25);
	gpioPort->AFR[0] |= (1U<<26);
	gpioPort->AFR[0] &=~ (1U<<27);

	gpioPort->AFR[0] |= (1U<<28);
	gpioPort->AFR[0] &=~ (1U<<29);
	gpioPort->AFR[0] |= (1U<<30);
	gpioPort->AFR[0] &=~ (1U<<31);
}

void SPI::configureSPI(){
	// 1. Enable clock to APB2 SPI
	RCC->APB2ENR |= SPI1EN;

	// 2. Set the clock of the SPI
	// Set the baudrate in the bits 5:3 of the SPI control register (CR1)
	// Set it to 001 to divide the clock by 4, so 16MHz/4 = 4MHz
	spi->CR1 |=  (1U<<3);
	spi->CR1 &=~ (1U<<4);
	spi->CR1 &=~ (1U<<5);

	// 3. Set the clock polarity and phase to 1
	// Set bits 1 and 0 of the SPI control regiser (CR1) to 1
	spi->CR1 |=  (1U<<0);
	spi->CR1 |=  (1U<<1);

	// 4. Enabe Full Duplex
	spi->CR1 &=~ (1U<<10);

	// 5. Set MSB FIRST (Frame Format: Most significant bit first)
	spi->CR1 &=~ (1U<<7);

	// 6. Set Master mode
	spi->CR1 |=  (1U<<2);

	// 7. Set data size (DFF: Data frame format). Set it to 0 for 8 bit.
	spi->CR1 &=~  (1U<<11);

	// 8. Set software slave management (SSM) and internal slave select (ISS)
	spi->CR1 |=  (1U<<9);
	spi->CR1 |=  (1U<<8);

	// 9. Enable SPI
	spi->CR1 |=  (1U<<6);
}

/*Enable chip select (cs)*/
void SPI::csEnable(){
	// we disable the csPin to enable cs, because by setting it to 0 (low) it's enabled
	gpioPort->ODR &= ~(1<<csPin);
}

/*Disable chip select (cs)*/
void SPI::csDisable(){
	// we enable the csPin to disable cs, because by setting it to 1 (high) it's disabled
	gpioPort->ODR |= (1<<csPin);
}

void SPI::transmit(uint8_t * data, uint32_t size){
	uint32_t i=0;
	uint8_t temp;

	while(i<size)
	{
		// Wait until buffer TXE flag (Tx buffer empty) in SPI_SR (status register) is set
		while(!(spi->SR & SR_TXE)){}

		// Write the data to the data register
		spi->DR = data[i];

		i++;
	}
	// Wait until buffer TXE flag (Tx buffer empty) in SPI_SR (status register) is set
	while(!(spi->SR & SR_TXE)){}

	// Wait for BUSY flag to reset = Wait while it is high
	while((spi->SR & SR_BSY)){}

	// Clear OVR flag (Overrun)
	// From the documentation: Clearing the OVR bit is done by a read operation on the SPI_DR register followed by
	// a read access to the SPI_SR register.
	temp = spi->DR;
	temp = spi->SR;
}

void SPI::receive(uint8_t * data, uint32_t size){
	while(size)
	{
		// NEW: Wait until TX buffer empty
		while (!(spi->SR & SR_TXE)) {}

		// Send dummy data
		spi->DR = 0;

		// Wait for RXNE flag (Receive buffer not empty) to be set = there is some data to read
		while(!(spi->SR & SR_RXNE)){}

		// Read data from data register
		*data++ = spi->DR;

		// decrement the size
		size--;
	}
}
