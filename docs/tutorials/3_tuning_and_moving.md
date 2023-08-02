# 03 - Tuning and Moving

## Introduction
Welcome to the third LemLib tutorial! In this tutorial, we will be learning how to tune the PIDs, move the robot, and use odometry.

## Odometry
As mentioned in the previous tutorial, LemLib uses odometry to track the position of the robot. However, we need to calibrate it at the start of each match. To do this, we need to call the `chassis.calibrate()` function in `initialize()` Below is an example of how to do this:
```cpp
void initialize() {
    chassis.calibrate();
}
```
Note that the chassis should be stationary when you call this function. It will take 3 seconds to calibrate, so don't be alarmed if it doesn't seem to be doing anything.



Pretty simple, right? Now, we can use the `chassis.getPose()` function to get the current position of the robot. It returns a `lemlib::Pose` object, which contains the x, y, and heading. The code below uses the `chassis.getPose()` function to print the current position of the robot to the brain screen:
```cpp
void screen() {
    // loop forever
    while (true) {
        lemlib::Pose pose = chassis.getPose(); // get the current position of the robot
        pros::lcd::print(0, "x: %f", pose.x); // print the x position
        pros::lcd::print(1, "y: %f", pose.y); // print the y position
        pros::lcd::print(2, "heading: %f", pose.theta); // print the heading
        pros::delay(10);
    }
}

void initialize() {
    chassis.calibrate(); // calibrate the chassis
    pros::Task screenTask(screen); // create a task to print the position to the screen
}
```

We can also set the position of the robot using the `chassis.setPose()` function. Below is an example of how to do this:
```cpp
void initialize() {
    chassis.calibrate(); // calibrate the chassis
    chassis.setPose(0, 0, 0); // X: 0, Y: 0, Heading: 0
    chassis.setPose(5.2, 10.333, 87); // X: 5.2, Y: 10.333, Heading: 87
}
```

## Moving with turnTo and moveTo

LemLib has 3 functions for moving the to. We will be covering the first 2 in this tutorial, and the third in the next tutorial.

The first function is `lemlib::Chassis::turnTo`. This function turns the robot so that it is facing the specified (x, y) point. It takes between 3 and 5 arguments. It uses the PID gains specified in the lateralController struct. Below is an example of how to use it:
```cpp
void autonomous() {
    chassis.turnTo(53, 53, 1000); // turn to the point (53, 53) with a timeout of 1000 ms
    chassis.turnTo(-20, 32, 1500, true); // turn to the point (-20, 32) with the back of the robot facing the point, and a timeout of 1500 ms
    chassis.turnTo(10, 0, 1000, false, 50); // turn to the point (10, 0) with a timeout of 1000 ms, and a maximum speed of 50
}
```

As you can see, using this function is very easy. The first 2 parameters are the X and Y location the robot should be facing. The third parameter is the timeout, which is the maximum time the robot can spend turning before giving up. The fourth parameter is whether the back of the robot should face the point (true) or the front of the robot should face the point (false). It defaults to false if not specified. The fifth parameter is the maximum speed the robot can turn at. If you don't specify a value for this parameter, the robot will turn at full speed.

The second function is `lemlib::Chassis::moveTo`. This function moves the robot to the specified (x, y) point with a target heading in degrees using an algorithm called the [Boomerang Controller](https://www.desmos.com/calculator/sptjw5szex). It uses the PID gains specified in the lateralController and angularController struct. Below is an example of how to use it:
```cpp
void autonomous() {
    chassis.moveTo(53, 53, 90, 1000); // move to the point (53, 53) at heading 90 with a timeout of 1000 ms
    chassis.moveTo(10, 0, 270, 1000, 50); // move to the point (10, 0) at heading 270 with a timeout of 1000 ms, and a maximum speed of 50
}
```

This function is very similar to the `chassis.turnTo()` function. The first 3 parameters are the x, y, and heading the robot should move to. The fourth parameter is the timeout, which is the maximum time the robot can spend turning before giving up. The fifth parameter is the maximum speed the robot can move at. If you don't specify a value for this parameter, the robot won't limit its speed.


## Tuning the PIDs
Now that we know how to move the robot, we can start tuning the PIDs. Let's start with the lateral PIDs.

The lateral PID is just a simple PD controller, with some minor optimizations. When we tune the lateral PD, we want the kP as high as possible with minimal oscillation. But how do we change these gains? The answer is the `lateralController` struct we created in the previous tutorial. Here is a reminder of what it looks like:
```cpp
// forward/backward PID
lemlib::ChassisController_t lateralController {
	10, // kP
	30, // kD
	1, // smallErrorRange
	100, // smallErrorTimeout
	3, // largeErrorRange
	500 // largeErrorTimeout
};
```

The first 2 parameters are the kP and kD gains. These are the ones we will be focusing on for now. When we tune them, we want kP as high as possible with minimal oscillation (the robot moving backwards/forwards repeatedly at the end). Here is the algorithm we will be using to tune these gains:

1. Move the robot 10 inches forward using the `chassis.moveTo()` function
2. increase kP until the robot starts oscillating
3. increase kD until the oscillation stops
4. record kP and kD values
5. repeat steps 2-4 until you can't stop the oscillation. At this point, use the last kP and kD values you recorded.

After this, you need to tune the slew rate. This controls the maximum acceleration of the chassis in order to prevent tipping. To tune it, simply increase it until the robot starts tipping too much. Higher values make the robot accelerate faster, and slower values make the robot accelerate slower. 

<br>

The process for the angular PID is very similar. Here is a reminder of what the angular PID looks like:
```cpp
// turn PID
lemlib::ChassisController_t angularController {
    2, // kP
    10, // kD
    1, // smallErrorRange
    100, // smallErrorTimeout
    3, // largeErrorRange
    500 // largeErrorTimeout
};
```

Here is the algorithm we will be using to tune these gains:

1. Turn the robot to face (30, 0) using the `chassis.turnTo()` function
2. increase kP until the robot starts oscillating
3. increase kD until the oscillation stops
4. record kP and kD values
5. repeat steps 2-4 until you can't stop the oscillation. At this point, use the last kP and kD values you recorded.

## Optional - Tuning Timeouts

You may have noticed that there are 4 more values in the angularController and lateralController structs. These are values for the timeouts. Here is how they work:

- `smallErrorRange` is the range of error that is considered "small". If the error is within this range for `smallErrorTimeout` milliseconds, the robot will proceed to the next movement
- `largeErrorRange` is the range of error that is considered "large". If the error is within this range for `largeErrorTimeout` milliseconds, the robot will proceed to the next movement

The units for error in the `chassis.moveTo()` function are inches, and degrees for the `chassis.turnTo()` function. The units for time are milliseconds.

Advanced users may wish to alter these values to decrease the time it takes to execute the next command. However, the default values should be fine for most users.

## Using the Path Generator for Coordinates

Another project we have been developing is the [Path Generator](https://lemlib.github.io/Path-Gen/). This web app allows you to create a path for the robot to follow using pure pursuit, which will be covered in the next tutorial. For now, we will be using it to view coordinates on the field. 

When hovering your mouse over a location on the field, its coordinates will be displayed in the bottom left corner. You can then manually input these coordinates into the `chassis.moveTo()` and `chassis.turnTo()` functions. You can also view the starting position of the robot by simply hovering your mouse over where it would start. You can set the position of the robot by using the `chassis.setPose()` function, as documented in [tutorial 2](2_setting_up_the_chassis.md). Below is an image of the Path Generator:

<img src="./assets/3_tuning_and_moving/path_generator.png">

Note that the origin of the field is in the middle, and the field coordinates are measured in inches. **0 degrees is facing up, and increases clockwise**.


Thats it! You now know how to move the robot around the field using the `chassis.turnTo()` and `chassis.moveTo()` functions. In the next tutorial, we will be covering how to use the Path Generator to create a path for the robot to follow.


[Previous Tutorial](2_setting_up_the_chassis.md) <br>
[Next Tutorial](4_pure_pursuit.md)
