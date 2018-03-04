/******************************************************************************
arduino_mpu9250_i2c.cpp - MPU-9250 Digital Motion Processor Arduino Library
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

This library implements motion processing functions of Invensense's MPU-9250.
It is based on their Emedded MotionDriver 6.12 library.
	https://www.invensense.com/developers/software-downloads/

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
******************************************************************************/
#include "arduino_mpu9250_i2c.h"
//#include <Arduino.h>
//#include <Wire.h>
// #include "application.h"
// #include "Particle.h"
#include "hFramework.h"


int arduino_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
// 	Wire.beginTransmission(slave_addr);
// 	Wire.write(reg_addr);
// 	for (unsigned char i = 0; i < length; i++)
// 	{
// 		Wire.write(data[i]);
// 	}
// 	Wire.endTransmission(true);

    uint8_t i2cTxBuf[length + 1];

	i2cTxBuf[0] = reg_addr;
	for (int i = 0; i < length; i++) {
		i2cTxBuf[i + 1] = data[i];
	}
	hSens2.i2c.write(slave_addr, i2cTxBuf, length + 1);

	return 0;
}

int arduino_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
// 	Wire.beginTransmission(slave_addr);
// 	Wire.write(reg_addr);
// 	Wire.endTransmission(false);
// 	Wire.requestFrom(slave_addr, length);
// 	for (unsigned char i = 0; i < length; i++)
// 	{
// 		data[i] = Wire.read();
// 	}
    hSens3.pin3.write(1);
    hSens3.pin4.write(1);
    hSens2.i2c.write(slave_addr, &reg_addr, 1);
    hSens3.pin4.write(0);
    hSens2.i2c.read(slave_addr, data, length);
    //hSens2.i2c.rw(slave_addr, &reg_addr, 1, data, length,0);
	hSens3.pin3.write(0);
	return 0;
}
