#include "gpio.hpp"

/*Syntax is ClassName : Function*/

/*Constructor
 * initialize the private class members before the constructor executes
 * initialize the port and pin with clock access and mode register*/
GPIO::GPIO(GPIO_TypeDef *_port, uint8_t _pin, bool isOutput):port(_port), pin(_pin)
{
	/*Enable clock access to GPIO*/
	if(port == GPIOA) {RCC->AHB1ENR |= GPIOAEN;}
	if(port == GPIOB) {RCC->AHB1ENR |= GPIOBEN;}
	if(port == GPIOC) {RCC->AHB1ENR |= GPIOCEN;}

	/*Configure pin*/
	if(isOutput)
	{
		/*Set mode as output 01*/
		port->MODER |=  (1<<(2*pin));
		port->MODER &=~ (1<<(2*pin + 1));
	}
	else
	{
		/*Set mode as input 00*/
		port->MODER &=~ (3<<(2*pin + 1));
	}
}

/*Function to toggle the pin*/
void GPIO::toggle()
{
	port->ODR ^= (1U<<pin);
}

/*Function to set the pin state low*/
void GPIO::set_low()
{
	port->ODR &=~ (1U<<pin);
}

/*Function to set the pin state high*/
void GPIO::set_high()
{
	port->ODR |= (1U<<pin);
}

/*Function to read the pin state*/
bool GPIO::read()
{
	return (port->IDR & (1U<<pin));
}
