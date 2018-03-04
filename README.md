# self-balancing-telepresence-robot
Self balancing telepresence robot is controlled worldwide from a web user interface avaiable at https://cloud.husarion.com/ . You can not only control this robot, but also see the image from the smartphone integrated with this robot with hVideo app (available in <a href="https://itunes.apple.com/us/app/hvideo/id1286551310?mt=8">App Store</a> or <a href="https://play.google.com/store/apps/details?id=com.husarion.video2">Google Play</a>)

## Main features of the robot ##

* web user interface
* holder for a smartphone or a small tablet (iPad mini is the biggest fitting tablet) with hVideo app (<a href="https://play.google.com/store/apps/details?id=com.husarion.video2">Android</a> or <a href="https://itunes.apple.com/us/app/hvideo/id1286551310?mt=8">iPhone/iPad</a>)
* stabilization algorithm immune to disruptions
* a servo motor controlled leg for balancing start/stop mode
* smooth driving
* a chassis preventing robot damage

## Robot hardware overview ##

![pid diagram](/img/ralph_alu_back_hd.jpg)

* based on Husarion CORE2 robotic controller with Arduino compatibility layer and RTOS
* 2 DC motors with quadrature encoders
* MPU9250 inertial measurement unit (gyro + accelerometer) and integrated DMP feature for complete 3D orientation data
* 1 servo for leg
* 3 x 18650 Li-ION batteries holder (up to 8h of operation without charging)

## Stabilizating algorithm overview ##

![pid diagram](/img/diagram_pid.png)

This robot uses 4 PID regulators:
* one for each DC motor with encoder for a motor speed regulation
* for holding desired angle between robot chassis and a ground
* for controlling motor angle to obtain desired speed of the whole robot


