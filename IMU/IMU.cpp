#include "hFramework.h"
#include "Arduino.h"
#include <SparkFunMPU9250-DMP.h>
#include "IMU.h"


MPU9250_DMP mpu;
hGPIO pin5V(91);

void quatVisualisation()
{
	static uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };
	static int i = 0;

	if (i == 0) {
		hFramework::Serial.init(115200);
		i = 1;
	}

	teapotPacket[2] = 0xff & (mpu.qw >> 24);
	teapotPacket[3] = 0xff & (mpu.qw >> 16);
	teapotPacket[4] = 0xff & (mpu.qx >> 24);
	teapotPacket[5] = 0xff & (mpu.qx >> 16);
	teapotPacket[6] = 0xff & (mpu.qy >> 24);
	teapotPacket[7] = 0xff & (mpu.qy >> 16);
	teapotPacket[8] = 0xff & (mpu.qz >> 24);
	teapotPacket[9] = 0xff & (mpu.qz >> 16);
	hFramework::Serial.write(teapotPacket, 14);
	teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
}

// We're going to have a super cool function now that gets called when a matching API request is sent
// This is the ledToggle function we registered to the "led" Particle.function earlier.
void printIMUData(void)
{
	// After calling dmpUpdateFifo() the ax, gx, mx, etc. values
	// are all updated.
	// Quaternion values are, by default, stored in Q30 long
	// format. calcQuat turns them into a float between -1 and 1
	float q0 = mpu.calcQuat(mpu.qw);
	float q1 = mpu.calcQuat(mpu.qx);
	float q2 = mpu.calcQuat(mpu.qy);
	float q3 = mpu.calcQuat(mpu.qz);

	sys.log("R/P/Y: %f,\t%f,\t%f\r\n", mpu.roll, mpu.pitch, mpu.yaw);
}

const signed char orientationDefault[9] = { 0, 1, 0, 0, 0, 1, 1, 0, 0 };

void IMU::begin()
{

	pin5V.write(0);
	sys.delay(50);
	pin5V.write(1);
	sys.delay(150);

	hSens2.pin1.interruptOn_EdgeRising();//interruptOn(InterruptEdge::Rising);

	if (mpu.begin() != INV_SUCCESS) {
		while (1) {
			sys.log("error MPU-9250\r\n");
			delay(7000);
		}
	}

	mpu.dmpBegin(DMP_FEATURE_6X_LP_QUAT  | // Enable 6-axis quat
	             DMP_FEATURE_GYRO_CAL, // Use gyro calibration
	             50); // Set DMP FIFO rate to 50 Hz (max 200 Hz)

	mpu.dmpSetOrientation(orientationDefault);
	// Use enableInterrupt() to configure the MPU-9250's
	// interrupt output as a "data ready" indicator.
	mpu.enableInterrupt();
	sys.log("lpf = %d", mpu.getLPF());
	mpu.setLPF(42); //188, 98, 42, 20, 10
	sys.log("lpf = %d", mpu.getLPF());

	for (int i = 0; i < 100; i++) {
		if (1 == hSens2.pin1.interruptWait(2000)) {
			while (mpu.fifoAvailable()) {
				if (mpu.dmpUpdateFifo() == INV_SUCCESS) {
					;
				}
			}
		} else {
			sys.log("MPU9250 read timeout\r\n");
		}
		LED3.toggle();
	}
}

float IMU::getAngle()
{
    static float retval = 0.0;
	if (1 == hSens2.pin1.interruptWait()) {
		while (mpu.fifoAvailable()) {
		    hSens3.pin2.write(1);
			// Use dmpUpdateFifo to update the ax, gx, mx, etc. values
			if (mpu.dmpUpdateFifo() == INV_SUCCESS) {
				// computeEulerAngles can be used -- after updating the
				// quaternion values -- to estimate roll, pitch, and yaw
				mpu.computeEulerAngles();
				//printIMUData();
				retval = mpu.roll;
				//	quatVisualisation();
			}
			hSens3.pin2.write(0);
		}
	}
	return retval;
}

void IMU::resetFifo()
{
    mpu.resetFifo();
}
