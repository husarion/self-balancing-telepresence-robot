#ifndef _IMU_H_
#define _IMU_H_

#include "Arduino.h"

class IMU
{
public:
    void begin();
    float getAngle();
    void resetFifo();
};

#endif