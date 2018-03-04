cmake_minimum_required (VERSION 2.6)
project(Main NONE)

set(BOARD_TYPE "core2")
set(PORT "stm32")
set(BOARD_VERSION "1.0.0")
include(${HFRAMEWORK_PATH}/hFramework.cmake)

enable_module("hCloudClient" "")
enable_module("hSensors" "")
enable_module("hModules" "")
enable_module("hROS" "")

include_directories("Controller")
include_directories("IMU/MPU9250lib")
include_directories("IMU")
add_hexecutable(main "IMU/IMU.cpp" "main.cpp" "IMU/MPU9250lib/SparkFunMPU9250-DMP.cpp" "IMU/MPU9250lib/inv_mpu_dmp_motion_driver.cpp" "IMU/MPU9250lib/inv_mpu.cpp" "IMU/MPU9250lib/arduino_mpu9250_log.cpp" "IMU/MPU9250lib/arduino_mpu9250_i2c.cpp" "IMU/MPU9250lib/arduino_mpu9250_clk.cpp" "Controller/wheel.cpp" "Controller/RobotController.cpp")
