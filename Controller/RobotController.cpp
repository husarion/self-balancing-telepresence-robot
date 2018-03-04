#include "RobotController.h"
#include "wheel.h"
#include "hCyclicBuffer.h"
#include "IMU.h"

#include <cstddef>
#include <cstdint>

#define ALPHA0_LOAD_FROM_STORAGE 1
#define PARKING_ON_SERVO_PUSLE_WIDTH 800
#define PARKING_OFF_SERVO_PUSLE_WIDTH 2100

//IMU
IMU imu;

//motors
Wheel wheelLeft(hMotD, 1);
Wheel wheelRight(hMotC, 0);
static IServo* servo = &hServoModule.servo1;


//PID angle
float angleNow = 0;
float targetAngle = 0;
float errAngle = 0;
float pidOut_IMU = 0.0;;

float angle0 = 2.0;
float angleParking = angle0 - 8.0;

// float imu_kp = 0.32;
// float imu_ki = 0.0012;
// float imu_kd = 3;
float imu_kp = 0.44;
float imu_ki = 0.002;
float imu_kd = 7.3;

hPID pidIMU(imu_kp, imu_ki, imu_kd);

//PID speed
float lastSpeed[4];
hCyclicBuffer<float> lastSpeeds(4, lastSpeed);

float pidOut_V;
float errSpeed;
float speedNow = 0;

float turnRight = 0;

float targetSpeed = 0;
// float V_kp = 2.8;
// float V_ki = 0.0028;
// float V_kd = 8.4;
float V_kp = 1.7;
float V_ki = 0.004;
float V_kd = 8.1;

hPID pidV(V_kp, V_ki, V_kd);

//other
bool motorOn = 1;
bool parkingOn = 0;
bool parkingNewState = 0;
//hRecursiveMutex semParking;
hSemaphore semParking;

uint32_t dt = 0;
uint32_t lastTime = 0;

//logs
hPrintfDev* lg1 = &devNull;
hPrintfDev* lg2 = &devNull;

void paramsTask()
{
	char c;
	uint16_t pulse = 520; //520 - open, 1600 - close

	while (1) {
		c = lg1->getch();
		lg1->printf("\r\n===========%d===========\r\n", sys.getRefTime());
		switch (c) {
		case 'q': imu_kp += 0.01; break;
		case 'a': imu_kp -= 0.01; break;

		case 'w': imu_ki += 0.0001; break;
		case 's': imu_ki -= 0.0001; break;

		case 'e': imu_kd += 0.1; break;
		case 'd': imu_kd -= 0.1; break;
		case 'c': angle0 = angleNow; lg1->printf("angle = %f\r\n", angle0); break;

		case 'z': angle0 += 1.0; break;
		case 'x': angle0 -= 1.0;; break;

		case 't': V_kp += 0.10; break;
		case 'g': V_kp -= 0.10; break;

		case 'y': V_ki += 0.00001; break;
		case 'h': V_ki -= 0.00001; break;

		case 'u': V_kd += 0.10; break;
		case 'j': V_kd -= 0.10; break;

		case 'o': pulse += 10; servo->setWidth(pulse); lg1->printf("servo pulse=%d\r\n", pulse); break;
		case 'l': pulse -= 10; servo->setWidth(pulse); lg1->printf("servo pulse=%d\r\n", pulse); break;

		case '3': targetSpeed += 0.5; lg1->printf("targetSpeed=%f\r\n", targetSpeed); break;
		case '4': targetSpeed -= 0.5; lg1->printf("targetSpeed=%f\r\n", targetSpeed); break;

		case '7': pidOut_IMU += 1.0; break;
		case '8': pidOut_IMU -= 1.0; break;

		case 'r':
			pidIMU.reset();
			break;
		case 'f': motorOn = !motorOn; break;

		}

		lg1->printf("imu_kp = %f , imu_ki = %f, imu_kd = %f, imu_scale = %f\r\n", imu_kp, imu_ki, imu_kd);
		lg1->printf("V_kp = %f , V_ki = %f, V_kd = %f, V_scale = %f\r\n", V_kp, V_ki, V_kd);

		pidIMU.setCoeffs(imu_kp, imu_ki, imu_kd);
		pidV.setCoeffs(V_kp, V_ki, V_kd);
	}
}

void logTask()
{
	uint32_t t = sys.getRefTime();
	while (1) {
		lg2->printf("%d;%d;%f;%f;%f;%f;%f;%f;%f\r\n", dt, lastTime, angle0, angleNow, targetAngle, pidOut_IMU , speedNow, targetSpeed, pidOut_V);
		sys.delaySync(t, 50);
	}
}
//===================================

void RobotController::regTask()
{
	imu.resetFifo();

	pidIMU.reset();
	pidOut_IMU = 0.0;

	pidV.reset();
	pidOut_V = 0.0;

	wheelLeft.reset();
	wheelRight.reset();

	semParking.take(0);
	parkingOn = 1;
	motorOn = 0;

	while (1) {
		dt = sys.getRefTime() - lastTime;
		lastTime = sys.getRefTime();

		angleNow = imu.getAngle();

		//compute mean speed
		speedNow = (wheelLeft.getSpeed() + wheelRight.getSpeed()) / 2;
		lastSpeeds.push_back(speedNow);
		speedNow = 0;
		for (int i = 0; i < lastSpeeds.size(); i++) {
			speedNow += lastSpeeds[i];
		}
		speedNow = speedNow / lastSpeeds.size();

		//PID to control robot speed
		errSpeed = speedNow - targetSpeed;
		pidOut_V = pidV.update(errSpeed, dt);

		//PID to control robot angle
		if (parkingOn == 0) {
			targetAngle = angle0 - pidOut_V;
		} else {
			targetAngle = angleParking - pidOut_V;
		}
		errAngle = angleNow - targetAngle;
		pidOut_IMU = pidIMU.update(errAngle, dt);

		if ((angleNow - angle0) > 30.0 || (angleNow - angle0) < -30.0) {
			pidIMU.reset();
			pidOut_IMU = 0.0;

			pidV.reset();
			pidOut_V = 0.0;

			wheelLeft.reset();
			wheelRight.reset();

			LED2.on();
		} else {
			LED2.off();
		}

		if (motorOn == 0) {
			wheelLeft.turnOff();
			wheelRight.turnOff();
		} else {
			wheelLeft.turnOn();
			wheelRight.turnOn();

			wheelLeft.setSpeed(pidOut_IMU + turnRight);
			wheelRight.setSpeed(pidOut_IMU - turnRight);

			wheelLeft.update(dt);
			wheelRight.update(dt);
		}
		LED1.toggle();
	}
}

void RobotController::parkingTask()
{
    uint32_t t;
	while (1) {
		if (semParking.take() == true) {
		    t = sys.getRefTime();
		    lg1->printf("semParking.take() == 1 ; parkingNewState = %d; time = %d\r\n", parkingNewState?1:0, t);
			if (parkingNewState == 1) {
				pidIMU.setRange(-1.5, 1.5); //max speed to motors
				pidIMU.setIRange(-0.5, 0.5);

				servo->setWidth(PARKING_ON_SERVO_PUSLE_WIDTH);
				sys.delay(600);
				parkingOn = 1;
				sys.delay(100);
				motorOn = 0;
			} else {
				parkingOn = 0;
				pidIMU.setIRange(0, 0);
				servo->setWidth(PARKING_OFF_SERVO_PUSLE_WIDTH);
				motorOn = 1;
				sys.delay(200);

				pidIMU.setRange(-7.5, 7.5); //max speed to motors
				pidIMU.setIRange(-4, 4);
			}
		}
	}
}


void RobotController::begin()
{
#if ALPHA0_LOAD_FROM_STORAGE == 1
	float storageVal = 0;
	sys.getStorage().load(0, storageVal);
	lg1->printf("angle0 in storage %f\r\n", storageVal);
	if (storageVal == 0x00) {

	} else {
		angle0 = storageVal;
		angleParking = angle0 - 8.0;
	}
	lg1->printf("angle0 in program %f\r\n", angle0);
#endif
	wheelLeft.begin();
	wheelRight.begin();

	hServoModule.enablePower();
	servo->setWidth(PARKING_ON_SERVO_PUSLE_WIDTH);
    
    lg1->printf("imu init ...");
	imu.begin();
	lg1->printf("done\r\n");

	pidIMU.setRange(-7.5, 7.5); //max speed to motors
	pidIMU.setIRange(-4, 4);
	pidIMU.setCoeffs(imu_kp, imu_ki, imu_kd);
	pidIMU.setScale(1);

	pidV.setRange(-20, 20); // max angle to pidIMU
	pidV.setIRange(-10, 10);
	pidV.setCoeffs(V_kp, V_ki, V_kd);
	pidV.setScale(1);

	lastTime = sys.getRefTime();
	angleNow = imu.getAngle();

	sys.taskCreate(regTask, 4, 1024);
	sys.taskCreate(parkingTask);

	sys.taskCreate(logTask);
	sys.taskCreate(paramsTask);
}

void RobotController::setSpeed(float speed, float right)
{
	targetSpeed = speed;
	turnRight = right;
}

void RobotController::enableMotors(bool state)
{
	motorOn = state;
}

float RobotController::calibrateIMU()
{
	angle0 = angleNow;
	sys.getStorage().store(0, angle0);
	lg1->printf("new angle0 = %f\r\n", angle0);
	return angle0;
}

void RobotController::setLogDev1(hPrintfDev& dev1)
{
	lg1 = &dev1;
}

void RobotController::setLogDev2(hPrintfDev& dev2)
{
	lg2 = &dev2;
}

void RobotController::enableParking(bool state)
{
	parkingNewState = state;
	semParking.give();
	lg1->printf("parking mode %s\r\n", state?"enabled":"disabled");
}
