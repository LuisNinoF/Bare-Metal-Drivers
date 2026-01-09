#include "adc.hpp"

/*Define macros*/
#define GPIOAEN		(1U<<0)
#define GPIOBEN		(1U<<1)
#define GPIOCEN		(1U<<2)
#define ADC1EN		(1U<<8)
#define ADC_CH1		(1U<<0)
#define AD_SEQ_LEN_1  0x00
#define CR2_ADCON	 (1U<<0)
#define CR2_CONT	 (1U<<1)
#define CR2_SWSTART	  (1U<<30)
#define SR_EOC		  (1U<<1)

/**Methods**/

/*Declare auxiliary methods*/
uint8_t adc_channel_from_pin(GPIO_TypeDef* port, uint8_t pin);

/*Constructor*/
adc_module::adc_module(ADC_TypeDef * _adc, GPIO_TypeDef * _gpioPort, uint8_t _pin): adc(_adc), gpioPort(_gpioPort), pin(_pin){
}

/*Initialization*/
void adc_module::init(){
	/****Configure the ADC GPIO Pin**/
	/*Enable clock access to GPIOx*/
	if(gpioPort == GPIOA){
		RCC->AHB1ENR |= GPIOAEN;
	}
	else if(gpioPort == GPIOB){
		RCC->AHB1ENR |= GPIOBEN;
	}
	else if(gpioPort == GPIOC){
		RCC->AHB1ENR |= GPIOCEN;
	}

	/*Set pin PXN mode to analog mode, where X: gpioPort and N: pin number*/
	gpioPort->MODER |= (1U<< (2*pin));
	gpioPort->MODER |= (1U<< (2*pin + 1));

	/****Configure the ADC Module**/
	/*Enable clock access to the ADC module*/
	RCC->APB2ENR |=ADC1EN;

	/*Set conversion sequence start*/
	adc->SQR3 = adc_channel_from_pin(gpioPort, pin);

	/*Set conversion sequence length*/
	adc->SQR1 = AD_SEQ_LEN_1;

	/*Enable ADC module*/
	adc->CR2 |=CR2_ADCON;
}

uint8_t adc_channel_from_pin(GPIO_TypeDef* port, uint8_t pin){
	if(port == GPIOA){
		switch(pin){
			case 0: return 0;
			case 1: return 1;
			case 2: return 2;
			case 3: return 3;
			case 4: return 4;
			case 5: return 5;
			case 6: return 6;
			case 7: return 7;
			default: return 255;
		}
	}
	else if(port == GPIOB){
		switch(pin){
			case 0: return 8;
			case 1: return 9;
			default: return 255;
		}
	}
	else if(port == GPIOC){
		switch(pin){
			case 0: return 10;
			case 1: return 11;
			case 2: return 12;
			case 3: return 13;
			case 4: return 14;
			case 5: return 15;
			default: return 255;
		}
	}
	return 255;
}

/*Start conversion*/
void adc_module::startConversion(){
	/*Enable continuous conversion*/
	adc->CR2 |=CR2_CONT;

	/*Start ADC conversion*/
	adc->CR2 |=CR2_SWSTART;
}

/*Read value from adc*/
uint32_t adc_module::read(){
	/*Wait for conversion to be complete*/
	while(!(adc->SR & SR_EOC)){}

	/*Read converted value*/
	return (adc->DR);
}



