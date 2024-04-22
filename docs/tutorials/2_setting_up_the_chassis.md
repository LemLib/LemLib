# 02 - Setting Up The Chassis

## Introduction

You have made it to the second tutorial! Now that you have LemLib installed, we can start using it. In this tutorial, we will be setting up the chassis.

The chassis has multiple components. LemLib will handle everything for you, but you need to tell it what components you have.

## The Drivetrain

A vital component of any robot is the drivetrain. The drivetrain is what allows the robot to move around the field. LemLib needs to know what motors are in the drivetrain. Lets start by setting up the motors.

In [PROS](https://pros.cs.purdue.edu/v5/api/cpp/index.html), setting up a motor is trivial. All you need to do is create a motor object, set its port, what cartridge its using, and if it is reversed or not. We need to do this for each motor on the drivetrain. Below is an example:

```cpp
pros::Motor left_front_motor(1, pros::E_MOTOR_GEARSET_06, false); // port 1, blue gearbox, not reversed
pros::Motor left_back_motor(2, pros::E_MOTOR_GEARSET_18, false); // port 2, green gearbox, not reversed
pros::Motor right_front_motor(3, pros::E_MOTOR_GEARSET_36, true); // port 3, red gearbox, reversed
pros::Motor right_back_motor(4, pros::E_MOTOR_GEARSET_36, true); // port 4, red gearbox, reversed
```

Now we need to group the left side and right side motors so that we can pass it to LemLib. To do this, we can use the `pros::MotorGroup` class. Here is an example:
```cpp
pros::MotorGroup left_side_motors({left_front_motor, left_back_motor});
pros::MotorGroup right_side_motors({right_front_motor, right_back_motor});
```

Now that we have the motors set up, we need to tell LemLib about the track width, wheel diameter, and wheel rpm of the drivetrain. Let's start with the track width. The track width is the distance between the left and right drivetrain wheels, as shown in the image below:

<img src="./assets/2_setting_up_the_chassis/track_width.png" height=400 style="display: block;margin-left: auto;margin-right: auto;">

We also need to tell LemLib the diameter of the wheels. LemLib has wheel presets you can use for this (e.g `lemlib::Omniwheel::NEW_4` for the new 4 inch wheels) After that, we need to tell LemLib the rpm of the wheels. If your drivetrain is not geared, then the rpm of the wheels is the same as the rpm of the motor cartridge. If it is geared, refer to [this spreadsheet](https://docs.google.com/spreadsheets/d/1RSoLv3tnpiCgFyHb0QayxK-42r9MgVRD_4QQmeFM618/edit#gid=0) to find the rpm of the wheels. And finally, we need to tell the robot how fast it can go around corners. If you have at least two traction wheels, you can start at 8, but if you don't have traction wheels, start at 2. We will tune this later.

Now that we have all the information we need, we can create a `lemlib::Drivetrain_t` struct to pass to LemLib. Below is an example:
```cpp
lemlib::Drivetrain drivetrain(&left_side_motors, // left motor group
                              &right_side_motors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              360, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);
``` 

## Odometry

A crucial component of a consistent autonomous is position tracking, commonly known as odometry. Odometry allows the robot to know where it is on the field. LemLib has built in odometry, and is compatible with most tracking wheel / inertial sensor combinations. To set up odometry, we need to tell LemLib what sensors we are using. Lets start with the tracking wheels.

Tracking Wheels are non-powered wheels that are used to track the movement of the robot. Below is a photo of a typical tracking wheel:

<img src="./assets/2_setting_up_the_chassis/tracking_wheel.png" height=400 style="display: block;margin-left: auto;margin-right: auto;">

A tracking wheel can rotate freely on a screw joint, and rubber bands pull it down so it makes consistent contact with the field tiles. Tracking wheels can be connected to either an Optical Shaft Encoder or a V5 Rotation Sensor. Both are supported by LemLib.

A gear ratio may be used to increase the resolution of the encoder. If you are using a gear ratio, you need to specify it to the `lemlib::TrackingWheel` constructor. Gear ratios are input/output, so if you have a 2:1 gear ratio, the actual ratio is 2.

The first step in setting up the tracking wheels is to create an object for the encoder. Below is an example of how to do this:
```cpp
// left tracking wheel encoder
pros::ADIEncoder left_enc('A', 'B', true); // ports A and B, reversed
// right tracking wheel encoder
pros::Rotation right_rot(1, false); // port 1, not reversed
// back tracking wheel encoder
pros::ADIEncoder back_enc('C', 'D', false); // ports C and D, not reversed
```

Next, we have to create a `lemlib::TrackingWheel` object. This contains information about the tracking wheel, such as the diameter and its offset from the tracking center. Below is an example of how to do this:
```cpp
// uses "left_enc" as the encoder. Old 2.75" wheel, 4.3" left of the tracking center, 2:1 gear ratio
lemlib::TrackingWheel left_tracking_wheel(&left_enc, lemlib::Omniwheel::OLD_275, -4.3, 2);
// uses "right_rot" as the encoder. New half-cut 2.75" wheel, 1.7" right of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel right_tracking_wheel(&right_rot, lemlib::Omniwheel::NEW_275_HALF, 1.7);
// uses "back_enc" as the encoder. Old 2.75" wheel, 4.5" back of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel back_tracking_wheel(&back_enc, lemlib::Omniwheel::OLD_275, -4.5);
```

Hold on, how far away from the tracking center is the tracking wheel? Turns out, its not the straight distance to the center of the robot, but only one component of it. Below is a diagram which shows the relationship between the tracking center and the tracking wheel:

<img src="./assets/2_setting_up_the_chassis/tracking_wheel_distance.png" height=800 style="display: block;margin-left: auto;margin-right: auto;">

Remember, vertical tracking wheels should have a negative offset if on the left of the tracking center, and horizontal tracking wheels should have a negative offset if in front of the tracking center.

Now, we can put all the tracking wheels together into a struct with `lemlib::OdomSensors_t`. This struct will be passed to the LemLib chassis. Below is an example of how to do this:
```cpp
// left tracking wheel encoder
pros::ADIEncoder left_enc('A', 'B', true); // ports A and B, reversed
// right tracking wheel encoder
pros::Rotation right_rot(1, false); // port 1, not reversed
// back tracking wheel encoder
pros::ADIEncoder back_enc('C', 'D', false); // ports C and D, not reversed

// uses "left_enc" as the encoder. Old 2.75" wheel, 4.3" left of the tracking center, 2:1 gear ratio
lemlib::TrackingWheel left_tracking_wheel(&left_enc, lemlib::Omniwheel::OLD_275, -4.3, 2);
// uses "right_rot" as the encoder. New half-cut 2.75" wheel, 1.7" right of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel right_tracking_wheel(&right_rot, lemlib::Omniwheel::NEW_275_HALF, 1.7);
// uses "back_enc" as the encoder. Old 2.75" wheel, 4.5" back of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel back_tracking_wheel(&back_enc, lemlib::Omniwheel::OLD_275, -4.5);

// inertial sensor
pros::Imu inertial_sensor(2); // port 2

// odometry struct
lemlib::OdomSensors sensors(&left_tracking_wheel, // vertical tracking wheel 1
                            &right_tracking_wheel, // vertical tracking wheel 2
                            &back_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &inertial_sensor // inertial sensor
);
```

You don't need all these sensors though. Even if you don't have any, you can still use LemLib. If you don't have a sensor, just set it to `nullptr`. Just remember that the more sensors you use, the more accurate the odometry will be.

## PIDs

Lemlib uses 2 PIDs to control the motion of the robot (except for pure pursuit). Every chassis will have different constants however, so you will need to tune them. More about that in the third tutorial. For now, just copy and paste the following code into your `main.cpp` file:
```cpp
// forward/backward PID
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            30, // derivative gain (kD)
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// turning PID
lemlib::ControllerSettings angularController(2, // proportional gain (kP)
                                             10, // derivative gain (kD)
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew). 0 means no limit
);
```

## Putting it all together

Below is everything we have done so far, all passed to the `lemlib::Chassis` constructor:
```cpp
// motors
pros::Motor left_front_motor(1, pros::E_MOTOR_GEARSET_06, false); // port 1, blue gearbox, not reversed
pros::Motor left_back_motor(2, pros::E_MOTOR_GEARSET_18, false); // port 2, green gearbox, not reversed
pros::Motor right_front_motor(3, pros::E_MOTOR_GEARSET_36, true); // port 3, red gearbox, reversed
pros::Motor right_back_motor(4, pros::E_MOTOR_GEARSET_36, true); // port 4, red gearbox, reversed

// motor groups
pros::MotorGroup left_side_motors({left_front_motor, left_back_motor});
pros::MotorGroup right_side_motors({right_front_motor, right_back_motor});

// drivetrain struct
lemlib::Drivetrain drivetrain(&left_side_motors, // left motor group
                              &right_side_motors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 3.25" omnis
                              360, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// left tracking wheel encoder
pros::ADIEncoder left_enc('A', 'B', true); // ports A and B, reversed
// right tracking wheel encoder
pros::Rotation right_rot(1, false); // port 1, not reversed
// back tracking wheel encoder
pros::ADIEncoder back_enc('C', 'D', false); // ports C and D, not reversed

// uses "left_enc" as the encoder. Old 2.75" wheel, 4.3" left of the tracking center, 2:1 gear ratio
lemlib::TrackingWheel left_tracking_wheel(&left_enc, lemlib::Omniwheel::OLD_275, -4.3, 2);
// uses "right_rot" as the encoder. New half-cut 2.75" wheel, 1.7" right of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel right_tracking_wheel(&right_rot, lemlib::Omniwheel::NEW_275_HALF, 1.7);
// uses "back_enc" as the encoder. Old 2.75" wheel, 4.5" back of the tracking center, 1:1 gear ratio (default)
lemlib::TrackingWheel back_tracking_wheel(&back_enc, lemlib::Omniwheel::OLD_275, -4.5);

// inertial sensor
pros::Imu inertial_sensor(2); // port 2

// odometry struct
lemlib::OdomSensors sensors(&left_tracking_wheel, // vertical tracking wheel 1
                            &right_tracking_wheel, // vertical tracking wheel 2
                            &back_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &inertial_sensor // inertial sensor
);

// forward/backward PID
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            30, // derivative gain (kD)
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// turning PID
lemlib::ControllerSettings angularController(2, // proportional gain (kP)
                                             10, // derivative gain (kD)
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew). 0 means no limit
);

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors);
```

Thats it! You have now set up the chassis. In the next tutorial, we will be making the chassis move during driver control


[Previous Tutorial](1_getting_started.md) <br>
[Next Tutorial](3_driver_control.md)
