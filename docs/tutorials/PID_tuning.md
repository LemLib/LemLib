# PID Tuning

## Introduction

PIDs are used in the majority of motion algorithms in LemLib. There are 2 PID Controllers used by LemLib: one for lateral motion, and one for angular motion. This tutorial will focus on tuning those PIDs.

## What is a PID?

```{tip}
If you are not familiar with PID, check out the [BLRS Wiki](https://wiki.purduesigbots.com/software/control-algorithms/pid-controller)
```

A PID is a controller. It controls a mechanism. It takes a numerical input, and returns a numerical output. The numerical input is a measurement from a sensor, and the numerical output is the power for an actuator (e.g a motor). 

## Lateral PID

Here is the PID settings we copy/pasted earlier:

```cpp
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);
```

### kP and kD

kP and kD is the most important settings, they are responsible for the majority of the movement. To tune them, we will start by disabling all other settings:

```cpp
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
                                              0, // anti windup
                                              0, // small error range, in inches
                                              0, // small error range timeout, in milliseconds
                                              0, // large error range, in inches
                                              0, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);
```

We will use `Chassis::turnToHeading` to tune the angular controller

```cpp
void opcontrol() {
    
    chassis.turnToHeading()
}