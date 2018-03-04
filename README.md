# self-balancing-telepresence-robot
Self balancing telepresence robot is controlled worldwide from a web user interface avaiable at https://cloud.husarion.com/ . You can not only control this robot, but also see the image from the smartphone integrated with this robot with hVideo app (available in <a href="https://itunes.apple.com/us/app/hconfig/id1283536270?mt=8">App Store</a> or <a href="https://play.google.com/store/apps/details?id=com.husarion.video2">Google Play</a>)

## Main features of this robotic platform: ##

* based on Husarion CORE2 robotic controller with Arduino compatibility layer and RTOS
* uses 2 DC motors with quadrature encoders
* uses MPU9250 inertial measurement unit (gyro + accelerometer) and integrated DMP feature for complete 3D orientation data
* holder for a smartphone or a small tablet (iPad mini is the biggest fitting tablet)
* a servo motor controlled leg to stop balancing
* a chassis preventing robot damage during algorithm developmet
* easy access to unused ports of CORE2 controller to add your own modules

## Stabilizating algorithm overview

This robot uses 4 PID regulators:
* one for each DC motor with encoder for a motor speed regulation
* for holding desired angle between robot chassis and a ground
* for controlling motor angle to obtain desired speed of the whole robot

![pid diagram](/img/diagram_pid.png)
