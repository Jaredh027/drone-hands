# drone-hands
This repo is dedicated to a project that allows a user to control a drone with their hands as the controller.

# Parts
(2) esp32s | (2) MPU-6050 - gyroscope/accelerometer | (1) drone/vehicle to control 

# Software
Arduino IDE (For programming esp32s) | LibrePilot (For calibrating drones flight controller)

# Overview
A regular drone usually consists of a couple of main parts that allows the user to control the motors
1) Transmitter (controller)
2) Reciever (translates transmitter data to data that can be read by the flight controller)
3) Flight Controller (sends reciever data to motor and stabalizes drone)

In my case I want my transmitter/controller to be my hands. Meaning when I move my hand upward/downward or side-to-side I want the drone to move with my hands movements.

# How to achieve this
What we need to do is mimic the controllers and reciever. 
This means sending our own data for the Throttle, Roll, Pitch, and Yaw using our hands.

Our hands movement will specifically be them being rotated as if you are using a doorknob for data such as Roll and Yaw, and them being titled up and down as if you are knocking on a door for the Throttle and Pitch.

Throttle  |  Left Hand  | Titled
Yaw       |  Left Hand  | Rotated
Pitch     |  Right Hand | Titled
Roll      |  Right Hand | Rotated

This table above is an overview of which hand (left or right) controls what funciton (throttle, pitch, etc) by which movement (rotation or tilt).

To track this data we need something that can track the rotation and tilt of our hands and this is where the MPU-6050 comes in. This device is an Inertial Measurement Unit (IMU) with a built-in 3-axis accelerometer and 3-axis gyroscope, which together measure linear acceleration and angular velocity.

