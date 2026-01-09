#ifndef SPI_HPP
#define SPI_HPP

#include "stm32f4xx.h"
#include <cstdint>

class SPI{

private:
	/*private members*/
	SPI_TypeDef * spi;
	GPIO_TypeDef * gpioPort;
	uint8_t csPin;

	/*private methods to configure GPIO and SPI*/
	void configureGPIO();
	void configureSPI();

public:
	/*Constructor to initialize SPI
	 *args: SPI peripheral, GPIO port for chip select (cs) and GPIO pin for chip select (cs)*/
	SPI(SPI_TypeDef * spi, GPIO_TypeDef * gpioPort, uint8_t csPin);

	/*Enable chip select (cs)*/
	void csEnable(void);

	/*Disable chip select (cs)*/
	void csDisable(void);

	/*Transmit data over SPI
	 *args: data pointer to the data buffer to transmit, and the size (in bytes) of the data to transmit*/
	void transmit(uint8_t * data, uint32_t size);

	/*Receive data over SPI
	 *args: data pointer to the data buffer to store, and the size (in bytes) of the data to receive*/
	void receive(uint8_t * data, uint32_t size);

};

#endif
