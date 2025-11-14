/*
 * Purpose of this program:
 * Show the use of three external drivers created in assembly language: adc, gpio output and uart transmission
 *
 * How it works?
 * It uses the adc driver to read sensor data and compare it to a threshold.
 * If it is above the threshold, the gpio output is set to high (led on) and a confirmation message is sent via uart.
 * If it is below the threshold, the gpio output is set to low (led off) and an error message is sent via uart.
 *
 * Created by: Luis Nino
 * Last revision: 2025.10.30
 *
 */

#include <stdint.h>

// Declare external functions from adc.s
extern void adc_init(void);
extern uint32_t adc_read(void);

// Declare external functions from gpio_out.s
extern void led_init(void);
extern void led_on(void);
extern void led_off(void);

// Declare external functions from uart_tx.S
extern void uart_init(void);
extern void uart_transmit(int ch);


// Redirect printf
int __io_putchar(int ch)
{
	uart_transmit(ch);
	return ch;
}


// Declare global variables
uint32_t sensor_data;
const uint32_t Threshold = 2500;


int main(void)
{
	// Initialize drivers in assembly language
	adc_init();
	led_init();
	uart_init();


	while(1)
	{
		// Read data
		sensor_data = adc_read();

		// Take action
		if(sensor_data > Threshold)
		{
			led_on();
			printf("Data transmission active. Sensor data = %d \n\r", sensor_data);
		}
		else
		{
			led_off();
			printf("Data transmission inactive due to low sensor data \n\r");
		}
	}
}
