#include "adxl345.hpp"

/*Define macros*/
#define DEVID_R			(0x00) 	// Device ID
#define DEVICE_ADDR		(0x53)	// Device Address
#define DATA_FORMAT_R	(0x31)	// Data format control
#define POWER_CTL_R		(0x2D)	// Power saving features control

#define DATA_START_ADDR	(0x32)
#define DATA_FORMAT_R	(0x31)

#define FOUR_G			(0x01)
#define RESET			(0x00)
#define SET_MEASURE_B	(0x08)

#define MULTI_BYTE_EN	0x40
#define READ_OPERATION 	0x80

#define G_FACTOR		0.0078f

/*Constructor
 *args: spi class reference (not a copy), and the device address*/
ADXL345::ADXL345(SPI& spi, uint8_t deviceAddress)
	:spi(spi), deviceAddress(deviceAddress), accel_x(0), accel_y(0), accel_z(0)
	{}


/*private methods*/
void ADXL345::writeRegister(uint8_t reg, uint8_t value){
	// Local buffer
	uint8_t data[2];

	// Enable multi-byte, and place address into buffer
	data[0] = reg|MULTI_BYTE_EN;

	// place data into buffer
	data[1] = value;

	// pull cs line low to enable slave
	spi.csEnable();

	// transmit data and address
	spi.transmit(data,2);

	// pull cs line high to disable slave
	spi.csDisable();
}

void ADXL345::readRegister(uint8_t reg, uint8_t * data, uint8_t size){
	// Set read operation
	reg |= READ_OPERATION;

	// Enable multi-byte
	reg |= MULTI_BYTE_EN;

	// pull cs line low to enable slave
	spi.csEnable();

	// send the address of the memory local where to read from
	spi.transmit(&reg, 1);

	// read items (size in bytes) of the accelerometer and store it
	spi.receive(data, size);

	// pull cs line high to disable slave
	spi.csDisable();
}

/*Initialize ADXL345 sensor*/
void ADXL345::init(){
	// 2. Set data format range to +/- 4g
	writeRegister(DATA_FORMAT_R, FOUR_G);

	// 3. Reset all bits
	writeRegister(POWER_CTL_R, RESET);

	// 4. Configure power control measure bit
	writeRegister(POWER_CTL_R, SET_MEASURE_B);
}

/*Read raw acceleration data and update internal variables*/
void ADXL345::updateAcceleration(){
	uint8_t data_rec[6];

	readRegister(DATA_START_ADDR, data_rec, 6);

	accel_x = (int16_t)((data_rec[1]<<8)|data_rec[0]);
	accel_y = (int16_t)((data_rec[3]<<8)|data_rec[2]);
	accel_z = (int16_t)((data_rec[5]<<8)|data_rec[4]);
}

/*Getter function for raw acceleration in x*/
int16_t ADXL345::getAccelX(){
	return accel_x;
}

/*Getter function for raw acceleration in y*/
int16_t ADXL345::getAccelY(){
	return accel_y;
}

/*Getter function for raw acceleration in z*/
int16_t ADXL345::getAccelZ(){
	return accel_z;
}

/*Getter function for g acceleration in x*/
float ADXL345::getAccelX_g(){
	return accel_x * G_FACTOR;
}

/*Getter function for g acceleration in y*/
float ADXL345::getAccelY_g(){
	return accel_y * G_FACTOR;
}

/*Getter function for g acceleration in z*/
float ADXL345::getAccelZ_g(){
	return accel_z * G_FACTOR;
}

