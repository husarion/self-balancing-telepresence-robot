#ifndef _ROBOTCONTROLLER_H_
#define _ROBOTCONTROLLER_H_

#include "hFramework.h"

class RobotController {
private:
	static void regTask();
	static void parkingTask();
public:
	void setLogDev1(hPrintfDev& dev1);
	void setLogDev2(hPrintfDev& dev2);

	void begin();
	void setSpeed(float speed, float turnRight = 0);
	void enableMotors(bool state);
	float calibrateIMU();
	void enableParking(bool state);
};

#endif
