# 04 - Tuning and Moving

## Introduction
In this tutorial, we will be learning how to tune the PIDs, move the robot autonomously, and use odometry.

## Odometry
As mentioned in the previous tutorial, LemLib uses odometry to track the position of the robot. However, we need to calibrate it at the start of each match. To do this, we need to call the `chassis.calibrate()` function in `initialize()` Below is an example of how to do this:
```cpp
void initialize() {
    chassis.calibrate();
}
```
Note that the chassis should be stationary when you call this function. It will take 3 seconds to calibrate, so don't be alarmed if it doesn't seem to be doing anything.



Pretty simple, right? Now, we can use the `chassis.getPose()` function to get the current position of the robot. It returns a `lemlib::Pose` object, which contains the x, y, and heading. The code below uses the `chassis.getPose()` function to print the current position of the robot to the brain screen in inches and degrees:
```cpp
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    // print odom values to the brain
    pros::Task screenTask([&]() {
        while (true) {
            lemlib::Pose pose = chassis.getPose(); // get chassis position
            pros::lcd::print(0, "X: %f", pose.x.convert(in));
            pros::lcd::print(1, "Y: %f", pose.y.convert(in));
            pros::lcd::print(2, "Theta: %f", pose.theta.convert(deg));
            pros::delay(10);
        }
    });
}
```

We can also set the position of the robot using the `chassis.setPose()` function. Below is an example of how to do this:
```cpp
void autonomous() {
    // this is where the robot start in auto
    // X: 5.2 inches, Y: 10.333 inches, Heading: 87 degrees
    chassis.setPose(5.2_in, 10.333_in, 87_deg);
}
```
It is highly recommended you set the starting position of the robot for each autonomous. Otherwise you won't be able to take advantage of the visualization in the path generator

## Moving with turnTo and moveToPose
LemLib has 3 functions for moving the to. We will be covering the first 2 in this tutorial, and the third in the next tutorial.

The first function is `chassis.turnTo()`. This function turns the robot so that it is facing the specified (x, y) point. It takes between 3 and 5 arguments. It uses the PID gains specified in the linearController struct. Below is an example of how to use it:
```cpp
void autonomous() {
    // turn towards the point (53 inches, 53 inches) with a timeout of 1 second
    chassis.turnTo(53_in, 53_in, 1_sec);
    // turn towards the point (-20 inches, 32 inches) with the back of the robot facing the point, and a timeout of 1.5 seconds
    chassis.turnTo(-20_in, 32_in, 1.5_sec, true);
    // turn towards the point (10 inches, 0 inches) with a timeout of 1 second, and a maximum speed of 50, with the front of the robot facing the point
    chassis.turnTo(10_in, 0_in, 1_sec, false, 50);
}
```

As you can see, using this function is very easy. The first 2 parameters are the X and Y location the robot should be facing. The third parameter is the timeout, which is the maximum time the robot can spend turning before giving up. The fourth parameter is whether the back of the robot should face the point (true) or the front of the robot should face the point (false). It defaults to false if not specified. The fifth parameter is the maximum speed the robot can turn at. If you don't specify a value for this parameter, the robot will turn at full speed.

The second function is `chassis.moveToPose()`. This function moves the robot to the specified (x, y) point with a target heading in degrees. It uses the PID gains specified in the linearController and angularController struct. Below is an example of how to use it:
```cpp
void autonomous() {
    // move to the point (53 inches, 53 inches) at heading 90 degrees with a timeout of 1 second
    chassis.moveToPose(53_in, 53_in, 90_deg, 1_sec);
    // move to the point (10 inches, 0 inches) at heading 270 degrees with a timeout of 1 second.
    // Move in reverse
    chassis.moveToPose(10_in, 0_in, 270_deg, 1_sec, false);
}
```

This function is very similar to the `chassis.turnTo()` function. The first 3 parameters are the x, y, and heading the robot should move to. The fourth parameter is the timeout, which is the maximum time the robot can spend turning before giving up. The fifth parameter is whether the robot should move backwards or forwards. The fifth parameter is the maximum speed the robot can move at. Only the first 4 parameters are required.

## Understanding asynchronous movements
By default all movements in lemlib run asynchronously. Put plainly, it runs in its own task. While the system is robust, it takes a little getting used to. Let's look at some examples:

```c++
pros::millis(); // returns 0000
chassis.moveToPose(0_in, 0_in, 0_deg, 1_sec);
pros::millis(); // returns 0000
```

As you can see, function calls after a movement function call are ran immediately afterwards. However, it will wait until the current movement is done before calling the next movement call. Lets look at the next example:

```c++
pros::millis(); // returns 0000
chassis.moveToPose(0_in, 0_in, 0_deg, 1_sec);
pros::millis(); // returns 0000
chassis.moveToPose(0_in, 0_in, 0_deg, 1_sec);
pros::millis(); // returns 1000
```

The program waits. But what if we want to wait until the robot has moved a certain distance? Well, there's a function for that
```c++
pros::millis(); // returns 0000
chassis.moveToPose(0_in, 20_in, 0_deg, 1_sec);
chassis.waitUntil(10_in); // wait until the chassis has travelled 10 inches
pros::millis(); // outputs 500
chassis.waitUntilDone(); // wait until the movement has been completed
pros::millis(); // outputs 1000
```

> _**IMPORTANT NOTE**_
<br>
> `chassis::waitUntil` and `chassis.waitUntilDone` work for all movements. The only difference is when you are using `chassis.turnTo`, the units will be Angles instead of Lengths. The waitUntil function accepts any unit, and may still work with incorrect ones.

This system will take a bit of getting used to, but it is very powerful. If you need additional examples, check out the [example project](https://github.com/LemLib/LemLib/blob/master/src/main.cpp), open a [discussion](https://github.com/LemLib/LemLib/discussions/new?category=q-a), or open a ticket in our [discord server](https://discord.gg/pCHr7XZUTj)


## Tuning the PIDs
Now that we know how to move the robot, we can start tuning the PIDs. Let's start with the linear PIDs.

The linear PID is just a simple PD controller, with some minor optimizations. When we tune the linear PD, we want the kP as high as possible with minimal oscillation. But how do we change these gains? The answer is the `linearController` struct we created in the previous tutorial. Here is a reminder of what it looks like:
```cpp
// forward/backward PID
lemlib::ControllerSettings<Length> linearController {
	10, // kP
	30, // kD
	1_in, // smallErrorRange
	100_ms, // smallErrorTimeout
	3_in, // largeErrorRange
	500_ms, // largeErrorTimeout,
    20 // slew (maximum acceleration)
};
```

The first 2 parameters are the kP and kD gains. These are the ones we will be focusing on for now. When we tune them, we want kP as high as possible with minimal oscillation (the robot moving backwards/forwards repeatedly at the end). Here is the method we will be using to tune these gains:

1. Move the robot 10 inches forward using the `chassis.moveToPose()` function
2. increase kP until the robot starts oscillating
3. increase kD until the oscillation stops
4. record kP and kD values
5. repeat steps 2-4 until you can't stop the oscillation. At this point, use the last kP and kD values you recorded.

After this, you need to tune the slew rate. This controls the maximum acceleration of the chassis in order to prevent tipping. To tune it, simply increase it until the robot starts tipping too much. Higher values make the robot accelerate faster, and slower values make the robot accelerate slower. 

<br>

The process for the angular PID is very similar. Here is a reminder of what the angular PID looks like:
```cpp
// turn PID
lemlib::ControllerSettings<Angle> angularController {
    2, // kP
    10, // kD
    1_deg, // smallErrorRange
    100_ms, // smallErrorTimeout
    3_deg, // largeErrorRange
    500_ms, // largeErrorTimeout
    80 // slew (maximum acceleration)

};
```

Here is the algorithm we will be using to tune these gains:

1. Turn the robot to face (30_in, 0_in) using the `chassis.turnTo()` function with the robot starting at (0_in, 0_in)
2. increase kP until the robot starts oscillating
3. increase kD until the oscillation stops
4. record kP and kD values
5. repeat steps 2-4 until you can't stop the oscillation. At this point, use the last kP and kD values you recorded.

## Optional - Tuning Timeouts

You may have noticed that there are 4 more values in the angularController and linearController structs. These are values for the timeouts. Here is how they work:

- `smallErrorRange` is the range of error that is considered "small". If the error is within this range for `smallErrorTimeout`, the robot will proceed to the next movement
- `largeErrorRange` is the range of error that is considered "large". If the error is within this range for `largeErrorTimeout`, the robot will proceed to the next movement

Advanced users may wish to alter these values to decrease the time it takes to execute the next command. However, the default values should be fine for most users.

## Using the Path Generator for Coordinates

You may be wondering how we know what coordinate the robot start at, and what the location is of a specific thing (e.g a goal). Thankfully, it is very easy. You can use [this software](https://lemlib.github.io/Path-Gen/). Just hover your mouse over a location on the field, and you will see the coordinates of the mouse on the field. Refer to the image below:

<img src="./assets/4_auto_and_tuning/path_coords.png">

You can use these coordinates to set the starting position of the robot, and use them with the `chassis.turnTo()` and `chassis.moveToPose()` functions.`
Note that the origin of the field is in the middle, and the field coordinates are measured in inches. **0 degrees is facing up, and increases clockwise**.

Thats it! You now know how to move the robot around the field using the `chassis.turnTo()` and `chassis.moveToPose()` functions. In the next tutorial, we will be covering how to use the Path Generator to create a path for the robot to follow.


[Previous Tutorial](3_driver_control.md) <br>
[Next Tutorial](5_pure_pursuit.md)
