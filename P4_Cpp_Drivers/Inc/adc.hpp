#ifndef ADC_HPP
#define ADC_HPP

#include "stm32f4xx.h"

class adc_module{
private:
	ADC_TypeDef* adc;
	GPIO_TypeDef * gpioPort;
	uint8_t pin;

public:
	/*Constructor*/
	adc_module(ADC_TypeDef * _adc, GPIO_TypeDef * _gpioPort, uint8_t _pin);

	/*Initialization*/
	void init();

	/*Start conversion*/
	void startConversion();

	/*Read value from adc*/
	uint32_t read();
};

#endif
