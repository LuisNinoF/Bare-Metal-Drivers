/*
 * Purpose of this program:
 * Show the use of three external drivers created in C language: adc, gpio output and uart transmission
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

/* include drivers */
#include "adc1.h"
#include "gpio_out.h"
#include "uart.h"


// Declare global variables
uint32_t sensor_data;
const uint32_t Threshold = 2500;


int main(void)
{
	// Initialize drivers in C language
	pa1_adc_init();
	GPIO_OUT_init();
	uart2_tx_init();


	while(1)
	{
		// Read data
		sensor_data = adc_read();

		// Take action
		if(sensor_data > Threshold)
		{
			GPIO_OUT_on();
			printf("Data transmission active. Sensor data = %d \n\r", sensor_data);
		}
		else
		{
			GPIO_OUT_off();
			printf("Data transmission inactive due to low sensor data \n\r");
		}
	}
}
