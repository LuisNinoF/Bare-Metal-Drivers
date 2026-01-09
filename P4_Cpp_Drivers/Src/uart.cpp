#include "uart.hpp"

#define GPIOAEN		(1U<<0)
#define UART2EN		(1U<<17)
#define SR_TXE		(1U<<7)
#define PERIPH_CLOCK 16000000

UART::UART(USART_TypeDef *_uart, uint32_t _baudrate):uart(_uart),baudrate(_baudrate)
{
	/*Enable clock access to UART2*/
	RCC->APB1ENR |=	UART2EN;

	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set the mode of PA2 to alternate function mode (10)*/
	GPIOA->MODER &= ~(3U << (2 * 2));
	GPIOA->MODER |=  (2U << (2 * 2));

	/*Set the mode of PA3 to alternate function mode (10)*/
	GPIOA->MODER &= ~(3U << (3 * 2));
	GPIOA->MODER |=  (2U << (3 * 2));

	/*Set alternate function type to AF7(UART2_TX) (to 0111)*/
	GPIOA->AFR[0] &= ~(0xFU << (2 * 4));
	GPIOA->AFR[0] |=  (0x7U << (2 * 4));

	/*Set alternate function type to AF7(UART2_RX) (to 0111)*/
	GPIOA->AFR[0] &= ~(0xFU << (3 * 4));
	GPIOA->AFR[0] |=  (0x7U << (3 * 4));

	/*Configure PA3 pull-up*/
	GPIOA->PUPDR &= ~(3U << (3 * 2));
	GPIOA->PUPDR |=  (1U << (3 * 2));

	/*Disable UART peripheral*/
	uart->CR1 &= ~USART_CR1_UE;

	/*Set Baudrate*/
	setBaudrate(PERIPH_CLOCK);

	/*Enable TX and RX*/
	uart->CR1 |= USART_CR1_TE | USART_CR1_RE;

	/*Enable UART peripheral*/
	uart->CR1 |= USART_CR1_UE;

}

void UART::setBaudrate(uint32_t periph_clk)
{
	uart->BRR = (periph_clk + (baudrate/2))/baudrate;
}

void UART::sendChar(char ch)
{
	/*Wait until data register is empty*/
	while(!(uart->SR & SR_TXE)){}

	/*write byte to transmit to the data register*/
	uart->DR = (ch & 0xFF);
}

void UART::sendStr(const char *str)
{
	/*send one byte at a time until '/0' is found*/
	while(*str)
	{
		sendChar(*str++);
	}
}

void UART::sendInteger(int num)
{
	/*define local variables*/
	char buffer[12];
	int i = 0;
	bool isNegative = false;

	/*check if negative number*/
	if(num < 0){
		isNegative = true;
		num = -num;
	}

	/*convert integer to string (reverse order)*/
	do{
		buffer[i++] = (num % 10) + '0';
		num /= 10;
	} while (num > 0);

	/*add negative sign if applies*/
	if(isNegative){
		buffer[i++] = '-';
	}

	/*null terminate the string*/
	buffer[i] = '\0';

	/*reverse the string*/
	for (int j = 0; j < i/2; ++j){
		char temp = buffer[j];
		buffer[j] = buffer[i - 1 - j];
		buffer[i - 1 - j] = temp;
	}

	/*send number as string null terminated*/
	sendStr(buffer);
}

void UART::sendFloat(float num, uint8_t decimalPlaces)
{
    char buffer[32];
    int i = 0;

    /* Handle sign */
    if (num < 0.0f) {
        buffer[i++] = '-';
        num = -num;
    }

    /* Integer part */
    uint32_t intPart = (uint32_t)num;

    /* Convert integer part (store reversed) */
    char intBuf[12];
    int intLen = 0;

    if (intPart == 0) {
        intBuf[intLen++] = '0';
    } else {
        while (intPart > 0) {
            intBuf[intLen++] = (intPart % 10) + '0';
            intPart /= 10;
        }
    }

    /* Copy integer part in correct order */
    for (int j = intLen - 1; j >= 0; j--) {
        buffer[i++] = intBuf[j];
    }

    /* Decimal point */
    buffer[i++] = '.';

    /* Fractional part */
    float frac = num - (float)((uint32_t)num);
    for (uint8_t d = 0; d < decimalPlaces; d++) {
        frac *= 10.0f;
        uint8_t digit = (uint8_t)frac;
        buffer[i++] = digit + '0';
        frac -= digit;
    }

    buffer[i] = '\0';
    sendStr(buffer);
}


