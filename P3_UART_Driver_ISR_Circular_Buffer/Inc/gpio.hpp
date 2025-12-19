#ifndef GPIO_HPP
#define GPIO_HPP

#include "stm32f4xx.h"

#define GPIOAEN (1U<<0)
#define GPIOBEN (1U<<1)
#define GPIOCEN	(1U<<2)

class GPIO{

	/*Make the port and pin private, so they cannot be modified externally*/
private:
	GPIO_TypeDef *port;
	uint8_t pin;

public:
	/*Constructor prototype*/
	GPIO(GPIO_TypeDef *_port, uint8_t _pin, bool isOutput = true);

	/*Function prototypes*/
	void toggle();
	void set_low();
	void set_high();
	bool read();
};

#endif
