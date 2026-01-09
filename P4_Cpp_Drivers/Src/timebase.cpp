#include "timebase.hpp"

/*Define macros*/
#define CTRL_ENABLE		(1U<<0)
#define CTRL_TICKINT	(1U<<1)
#define CTRL_CLCKSRC	(1U<<2)
#define CTRL_COUNTFLAG	(1U<<16)
#define ONE_SEC_LOAD	16000000
#define MAX_DELAY		0xFFFFFFFF
#define TICK_FREQ		1

/*Initialize tick Counter*/
volatile uint32_t Timebase::tickCount = 0;

/**/
void Timebase::init(void)
{

	/*Disable global interrupts*/
	__disable_irq();

    /*Load the timer with number of clock cycles per second*/
	SysTick->LOAD =  ONE_SEC_LOAD - 1;

    /*Clear systick current value register*/
	SysTick->VAL = 0;

    /*Select internal clock source*/
	SysTick->CTRL = CTRL_CLCKSRC;

    /*Enable interrupt*/
	SysTick->CTRL |= CTRL_TICKINT;

	/*Enable systick*/
	SysTick->CTRL |=CTRL_ENABLE;

	/*Enable global interrupts*/
	__enable_irq();
}

void Timebase::delay(uint32_t delay)
{
	uint32_t tickstart =  getTick();
	uint32_t wait =  delay;

	if(wait < MAX_DELAY)
	{
		wait += (uint32_t)TICK_FREQ;
	}

    while((getTick() - tickstart) < wait){}
}

uint32_t Timebase::getTick(void)
{
	__disable_irq();
	uint32_t currentTick = tickCount;
	__enable_irq();

	return currentTick;
}

void Timebase::tickIncrement(void)
{
	tickCount += TICK_FREQ;
}

extern "C" void SysTick_Handler(void)
{
	Timebase::tickIncrement();
}


