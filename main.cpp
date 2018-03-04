#include "RobotController.h"
#include "hCloudClient.h"

RobotController rc;

float speed = 0;

void cfgHandler()
{
	auto l1 = platform.ui.label("l1");
	auto g1 = platform.ui.button("g1");
	auto g2 = platform.ui.button("g2");
	auto g3 = platform.ui.button("g3");
	auto stop = platform.ui.button("stop");

	platform.ui.loadHtml({Resource::WEBIDE, "/WebUI/ui.html"});
	platform.ui.video.enable();
}

void onKeyEvent(KeyEventType type, KeyCode code)
{
	static float right = 0;
	static float gear = 1.0;

	static bool motorState = 1;
	static bool parkingState = 1;

	static bool up_on = false;
	static bool down_on = false;
	static bool left_on = false;
	static bool right_on = false;

	platform.ui.console("cl1").printf("\r\nKE: t:%d, c:%d [%dms]", type, code, sys.getRefTime());

	bool isPressed = (type == KeyEventType::Pressed);
	if (code == KeyCode::Key_W || code == KeyCode::Up)
		up_on = isPressed;
	if (code == KeyCode::Key_S || code == KeyCode::Down)
		down_on = isPressed;
	if (code == KeyCode::Key_A || code == KeyCode::Left)
		left_on = isPressed;
	if (code == KeyCode::Key_D || code == KeyCode::Right)
		right_on = isPressed;

	if (type == KeyEventType::Pressed) {
		LED1.toggle();

		switch (code) {
		case KeyCode::Key_U: {
			gear = 0.5;
			break;
		}
		case KeyCode::Key_I: {
			gear = 1.0;
			break;
		}
		case KeyCode::Key_O: {
			gear = 2.0;
			break;
		}
		case KeyCode::Key_F: {
			motorState = !motorState;
			rc.enableMotors(motorState);
			platform.ui.console("cl1").printf("\r\nmotor %s", motorState ? "on" : "off");
			break;
		}
		case KeyCode::Key_P: {
			parkingState = !parkingState;
			rc.enableParking(parkingState);
			if(parkingState == 1) {
			    platform.ui.button("stop").setText("start");
			} else {
			    platform.ui.button("stop").setText("stop");
			}
			platform.ui.console("cl1").printf("\r\nparking %s", parkingState ? "on" : "off");
			break;
		}
		case KeyCode::Key_C: {
			float angle = rc.calibrateIMU();
			platform.ui.console("cl1").printf("\r\ncalibrate IMU, angle = %f", angle);
			break;
		}
		default : break;
		}
	}

	if (up_on) {
		speed = gear;
	} else {
		if (down_on) {
			speed = -gear;
		} else {
			speed = 0.0;
		}
	}
	if (left_on) {
		right = -0.5;
	} else {
		if (right_on) {
			right = 0.5;
		} else {
			right = 0.0;
		}
	}
	platform.ui.console("cl1").printf("\r\nspeed = %f, right = %f", speed, right);
	rc.setSpeed(speed, right);
}

void onButtonEvent(hId id, ButtonEventType type)
{
	//make unified interface and use onKeyEvent after mapping id and type

	platform.ui.console("cl1").printf("\r\nBE: t:%d, i:%s [%dms]", type, id.str(), sys.getRefTime());
	KeyEventType t = ((type == ButtonEventType::Pressed) ? KeyEventType::Pressed : KeyEventType::Released);
	KeyCode c = KeyCode::Key_X;

	if (id == "g1") {c = KeyCode::Key_U;}
	if (id == "g2") {c = KeyCode::Key_I;}
	if (id == "g3") {c = KeyCode::Key_O;}
	
	if (id == "stop") {c = KeyCode::Key_P;}

	if (id == "move_up") {c = KeyCode::Key_W;}
	if (id == "move_down") {c = KeyCode::Key_S;}
	if (id == "move_left") {c = KeyCode::Key_A;}
	if (id == "move_right") {c = KeyCode::Key_D;}

	onKeyEvent(t, c);
}


void statusTask()
{
	while (1) {
		platform.ui.label("lb_bat").setText("%2f", sys.getSupplyVoltage());
		sys.delay(2000);
	}
}

void hMain()
{
	// web UI handlers
	platform.begin(&RPi);
	platform.ui.configHandler = cfgHandler;
	platform.ui.onKeyEvent = onKeyEvent;
	platform.ui.onButtonEvent = onButtonEvent;
	platform.ui.setProjectId("@@@PROJECT_ID@@@");

	sys.taskCreate(statusTask);

	// logs from robot control algorithm
	hSens3.serial.init(115200);
	hSens4.serial.init(115200);

	rc.setLogDev1(hSens3.serial);
	rc.setLogDev2(hSens4.serial);

	// enable robot controller
	rc.begin();

	for (;;) {
		LED3.toggle();
		sys.delay(100);
	}
}