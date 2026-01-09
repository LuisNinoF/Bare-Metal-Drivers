/*
 * Purpose of this program:
 * Demonstrate modular C++ drivers for STM32F4 peripherals: GPIO, ADC, Systick, UART, SPI
 *
 * How it works?
 * Reads and sends sensor data from ADC and SPI modules using object-oriented programming (OOP).
 *
 * Uses:
 * Object-oriented peripheral abstraction for scalable embedded firmware development and sensor data telemetry.
 *
 * Created by: Luis Nino
 * Last revision: 2026.01.09
 *
 */

#include "stm32f4xx.h"
#include "gpio.hpp"
#include "uart.hpp"
#include "fpu.hpp"
#include "timebase.hpp"
#include "adc.hpp"
#include "spi.hpp"
#include "adxl345.hpp"

/*Declare prototype of helper function*/
void acceleration_print(UART& uart2, ADXL345& adxl345);

/*Run main*/
int main()
{
	/*Create led from class GPIO for PA5*/
	GPIO led(GPIOA, 5, true);

	/*Create button from class GPIOA for PC13*/
	GPIO button(GPIOC, 13, false);

	/*Initialize UART2*/
	UART uart2(USART2,115200);

	/*Enable FPU*/
	FPU_CP::enable();

	/*Initialize Systick*/
	Timebase::init();

	/*Create ADC object*/
	adc_module adc1(ADC1, GPIOA, 1);

	/*Initialize ADC object*/
	adc1.init();

	/*Start conversion for ADC object*/
	adc1.startConversion();

	/*Create SPI object*/
	SPI spi1(SPI1, GPIOA, 9);

	/*Create ADXL345 object and initialize it*/
	ADXL345 adxl345(spi1);
	adxl345.init();

	while(1)
	{
		/*toggle led if button is pushed*/
		if(!button.read())
		{
			led.toggle();
		}

		/*Read ADC value and send via uart*/
		uart2.sendStr("ADC Value: ");
		uart2.sendInteger(adc1.read());
		uart2.sendStr("\r\n");
		Timebase::delay(1);

		/*Read acceleration values from ADXL345 and send via uart*/
		adxl345.updateAcceleration();
		acceleration_print(uart2, adxl345);
		Timebase::delay(1);
	}

	return 1;
}




/*helper function to print all acceleration values*/
void acceleration_print(UART& uart2, ADXL345& adxl345){
	uart2.sendStr("X Acceleration Value (raw value): ");
	uart2.sendInteger(adxl345.getAccelX());
	uart2.sendStr(", X Acceleration Value (g units): ");
	uart2.sendFloat(adxl345.getAccelX_g());
	uart2.sendStr("\r\n");

	uart2.sendStr("Y Acceleration Value (raw value): ");
	uart2.sendInteger(adxl345.getAccelY());
	uart2.sendStr(", Y Acceleration Value (g units): ");
	uart2.sendFloat(adxl345.getAccelY_g());
	uart2.sendStr("\r\n");

	uart2.sendStr("Z Acceleration Value (raw value): ");
	uart2.sendInteger(adxl345.getAccelZ());
	uart2.sendStr(", Z Acceleration Value (g units): ");
	uart2.sendFloat(adxl345.getAccelZ_g());
	uart2.sendStr("\r\n");
}
