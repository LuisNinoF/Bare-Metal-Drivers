/*
 * Purpose of this program:
 * Demonstrate an interrupt driven UART driver in C++ using RX and TX circular buffers.
 *
 * How it works?
 * This program uses classes for: GPIOs, Circular Queues and UART Drivers.
 * Data transmission is done in software and hardware level using the UART2.
 *
 * At software level, two circular queues are used, one for RX and another one for TX.
 * User code enqueues bytes to the TX buffer to send, and dequeues bytes from the RX buffer to receive data.
 *
 * At hardware (interrupt) level, the UART IRQ handler calls RX and TX isr handlers.
 * To send data, the TX handler takes bytes from the TX buffer and writes them to the UART data register until the buffer is empty.
 * To receive data, the RX handler reads from the UART data register and pushes bytes into the RX buffer.
 *
 * Uses:
 * reliable, non-blocking serial communication, e.g. debug console, sensor data streaming, inter-process communication.
 *
 * Created by: Luis Nino
 * Last revision: 2025.12.18
 *
 */

#include "stm32f4xx.h"
#include "gpio.hpp"
#include "uart.hpp"

/*Create instance of UART driver*/
emb::drivers::UartDriver uart2(USART2);

int main()
{
	/*Initialize uart driver and notify*/
	if(!uart2.init(115200))
	{
		while(1)
		{
			uart2.send_string("Uart2 with Circular Buffer in C++ initialization failed \r\n");
		}
	}
	else
	{
		uart2.send_string("Uart2 with Circular Buffer in C++ initialized successfully \r\n");
	}

	/*Ask the user for a message to test the receive function*/
	uart2.send_string("Type to test receive function: \r\n");

	/*Declare local variable to store received byte*/
	uint8_t input_byte;

	while(1)
	{
		/*Check if a byte has been received and echo it back*/
		if(uart2.receive_byte(input_byte))
		{
			uart2.send_byte(input_byte);
		}
	}

	return 1;
}
