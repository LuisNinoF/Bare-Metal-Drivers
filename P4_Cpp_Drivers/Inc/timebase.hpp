#ifndef TIMEBASE_HPP
#define TIMEBASE_HPP

#include "stm32f4xx.h"

class Timebase{
private:
	static volatile uint32_t tickCount;

public:

	/*Initialize Systick*/
	static void init();

	/*Delay function according to delay argument in seconds*/
	static void delay(uint32_t delay);

	/*Get the current tick Count*/
	static uint32_t getTick();

	/*Increase the tick Counter by one, which is called inside the Sytick interrupt*/
	static void tickIncrement();

};

#endif
