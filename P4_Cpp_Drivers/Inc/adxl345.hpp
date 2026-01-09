#ifndef ADXL345_HPP
#define ADXL345_HPP

#include <cstdint>
#include "spi.hpp"

class ADXL345{

private:
	/*private members*/
	SPI& spi;
	uint8_t deviceAddress;
	int16_t accel_x, accel_y, accel_z;

	/*private methods*/
	void writeRegister(uint8_t reg, uint8_t value);
	void readRegister(uint8_t reg, uint8_t * data, uint8_t size);

public:
	/*Constructor
	 *args: spi class reference (not a copy), and the device address*/
	ADXL345(SPI& spi, uint8_t deviceAddress = 0x53);

	/*Initialize ADXL345 sensor*/
	void init();

	/*Read raw acceleration data and update internal variables*/
	void updateAcceleration();

	/*Getter function for raw acceleration in x*/
	int16_t getAccelX();

	/*Getter function for raw acceleration in y*/
	int16_t getAccelY();

	/*Getter function for raw acceleration in z*/
	int16_t getAccelZ();

	/*Getter function for g acceleration in x*/
	float getAccelX_g();

	/*Getter function for g acceleration in y*/
	float getAccelY_g();

	/*Getter function for g acceleration in z*/
	float getAccelZ_g();

};

#endif
