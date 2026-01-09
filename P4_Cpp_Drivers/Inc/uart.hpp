#ifndef UART_HPP
#define UART_HPP

#include "stm32f4xx.h"

class UART{

private:
	/*Define private members*/
	USART_TypeDef * uart;
	uint32_t baudrate;

	/*Set UART baudrate*/
	void setBaudrate(uint32_t periph_clk);

public:
	/*Constructor*/
	UART(USART_TypeDef *_uart, uint32_t _baudrate);

	/*Send byte*/
	void sendChar(char ch);

	/*Send string until the null terminator is found '/0'*/
	void sendStr(const char *str);

	/*Send integer calling sendStr inside*/
	void sendInteger(int num);

	/*Send float*/
	void sendFloat(float num, uint8_t decimalPlaces = 2);

};

#endif
