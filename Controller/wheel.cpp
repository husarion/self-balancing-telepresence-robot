#include "wheel.h"


Wheel::Wheel(hMotor& motor, bool polarity)
{
	mot = &motor;
	pol = polarity;
}

void Wheel::begin()
{
	//dzialaly 01.03.2018
	vReg.setScale(2);
	vReg.setKP(100.0);
	vReg.setKI(0.5);
	vReg.setKD(200.0);


// 	vReg.setScale(2);
// 	vReg.setKP(100.0);
// 	vReg.setKI(0.5); //0.5 - ok
// 	vReg.setKD(400.0);

	vReg.setRange(-vRange, vRange);

	if (1 == pol) {
		mot->setMotorPolarity(Polarity::Reversed);
		mot->setEncoderPolarity(Polarity::Reversed);
	}

	mot->resetEncoderCnt();
}

void Wheel::update(uint32_t dt)
{
	float vErr = 0.0;
	int32_t pidOut = 0;
	dNow = mot->getEncoderCnt();
	vNow = (dNow - lastPositions[0]) / (dt * lastPositions.size());
	lastPositions.push_back(dNow);

	vErr = vNow - vTarget;
	pidOut = vReg.update(vErr, dt);

	if (turnedOn == true) {
		mot->setPower(pidOut);
	}
}

void Wheel::setSpeed(float speed)
{
	vTarget = speed;
}

float Wheel::getSpeed()
{
	return vNow;
}

int32_t Wheel::getDistance()
{
	return dNow;
}

void Wheel::resetDistance()
{
	mot->resetEncoderCnt();
}

void Wheel::reset()
{
	mot->resetEncoderCnt();
	vReg.reset();
	dNow = 0;
	vNow = 0;
	mot->setPower(0);
}

void Wheel::turnOff()
{
	turnedOn = false;
	mot->setPower(0);
}
void Wheel::turnOn()
{
	turnedOn = true;
}
