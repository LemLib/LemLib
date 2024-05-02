# 2 - Setting Up The Chassis

## Introduction

Now that LemLib has been installed, we need to configure it before we can start using it. Most mistakes happen during configuration, so pay close attention to the instructions.

## The Drivetrain

First, we need to configure the motors on our drivetrain. Each motor has 3 properties: the port its connected to, whether its reversed or not, and what cartridge is installed (AKA gearbox).

```{important}
Motors are on the left side of the robot from the robot's point of view.
```

Let's start off by identifying the motors on the robot. We need to figure out what ports they are connected to. This can be done by entering the devices menu on the brain, and observing what port disappears when you unplug a motor. Record what motor is connected to what port. Now, instantiate the motor objects:

```{important}
Motors should be created outside of a function, near the top of the file.
```

```{seealso}
[pros::Motor documentation](https://pros.cs.purdue.edu/v5/api/cpp/motors.html)
```

```cpp
pros::Motor front_left_motor(1); // front left motor on port 1
pros::Motor middle_left_motor(2); // middle left motor on port 2
pros::Motor back_left_motor(3); // back left motor on port 3
pros::Motor front_right_motor(4); // front right motor on port 4
pros::Motor middle_right_motor(5); // middle right motor on port 5
pros::motor back_right_motor(6); // back right motor on port 6
```

Now, we need to determine which way the motors spin when we apply a positive voltage. This can be done by moving the motor through the devices menu on the brain screen and observing how the drive wheels move. See the table below for determining whether a motor is reversed or not:

```{important}
This needs to be done for all motors on the drivetrain
```

|                   | **Clockwise**         | **Counter-Clockwise** |
| ----------------- | --------------------- | --------------------- |
|    **Left**       | Reversed              | Forwards              |
|    **Right**      | Forwards              | Reversed              |

Now, let's update our configuration. If a motor is reversed, it has a negative port. If its forwards (not reversed), it has a positive port:

```cpp
pros::Motor front_left_motor(-1); // reversed
pros::Motor middle_left_motor(-2); // reversed
pros::Motor back_left_motor(-3); // reversed
pros::Motor front_right_motor(4); // forwards
pros::Motor middle_right_motor(5); // forwards
pros::motor back_right_motor(6); // forwards
```

Now, we need to specify what cartridge is used by every motor. The cartridge can be checked by looking at the area below the shaft of the motor. A motor can have one of three cartridges:

|          **Color**         |         **RPM**         | **Code Representation**    |
| -------------------------- | ----------------------- | -------------------------- | 
| **Red**                    |           100           | `pros::E_MOTOR_GEARSET_36` |
| **Green**                  |           200           | `pros::E_MOTOR_GEARSET_18` |
| **Blue**                   |           600           | `pros::E_MOTOR_GEARSET_06` |

We need to specify the cartridge configuration in code as well:

```cpp
pros::Motor front_left_motor(-1, pros::E_MOTOR_GEARSET_18); // green cartridge
pros::Motor middle_left_motor(-2, pros::E_MOTOR_GEARSET_06); // blue cartridge
pros::Motor back_left_motor(-3, pros::E_MOTOR_GEARSET_36); // red cartridge
pros::Motor front_right_motor(4, pros::E_MOTOR_GEARSET_18); // green cartridge
pros::Motor middle_right_motor(5, pros::E_MOTOR_GEARSET_06); // blue cartridge
pros::motor back_right_motor(6, pros::E_MOTOR_GEARSET_36); // red cartridge
```

Now, all our motors are configured. However, we need to add them to motor groups so LemLib can interface with them. See the code below:

```{seealso}
[pros::MotorGroup documentation](https://pros.cs.purdue.edu/v5/api/cpp/motor_groups.html)
```

```cpp
pros::Motor front_left_motor(-1, pros::E_MOTOR_GEARSET_18); // left_motor_group
pros::Motor middle_left_motor(-2, pros::E_MOTOR_GEARSET_06); // left_motor_group
pros::Motor back_left_motor(-3, pros::E_MOTOR_GEARSET_36); // left_motor_group
pros::Motor front_right_motor(4, pros::E_MOTOR_GEARSET_18); // right_motor_group
pros::Motor middle_right_motor(5, pros::E_MOTOR_GEARSET_06); // right_motor_group
pros::motor back_right_motor(6, pros::E_MOTOR_GEARSET_36); // right_motor_group

// left motor group
pros::MotorGroup left_motor_group({ front_left_motor, middle_left_motor, back_left_motor });
// right motor group
pros::MotorGroup right_motor_group({ front_right_motor, middle_right_motor, back_right_motor });
```

Now that our motors are fully configured, we need to pass them to LemLib. We can do this through a helper class: `Drivetrain`. It needs the following information:
 - Left motor group
 - Right motor group
 - track width
 - wheel diameter
 - horizontal drift

##### Track Width

Track width is the distance from the left side of the drivetrain to the right side of the drivetrain. Specifically, from the middle of the wheels. Track width is shown in the diagram below:

![track width](../assets/2_configuration/track_width.png)

Record the track width, we'll need it soon.

#### Wheel Diameter

This one should be self-explanatory. Its the diameter of the wheels on your drivetrain. The diameter on the wheels are actually slightly different than the diameter advertised by Vex (and, as is typical of Vex, this is not documented anywhere). For that reason, LemLib includes constants for all the different wheels, as follows:

```{note}
If, for whatever reason, you want a custom wheel size: you can input a number instead of a constant
```

| **Wheel Type**     | **Actual Size** | **LemLib Constant**               |
| ------------------ | --------------- | --------------------------------- |
| New 2"             | 2.125           | `lemlib::Omniwheel::NEW_2`        |
| New 2.75"          | 2.75            | `lemlib::Omniwheel::NEW_275`      |
| Old 2.75"          | 2.75            | `lemlib::Omniwheel::OLD_275`      |
| Half-Cut New 2.75" | 2.744           | `lemlib::Omniwheel::NEW_275_HALF` |
| Half-Cut Old 2.75" | 2.74            | `lemlib::Omniwheel::OLD_275_HALF` |
| New 3.25"          | 3.25            | `lemlib::Omniwheel::NEW_325`      |
| Old 3.25"          | 3.25            | `lemlib::Omniwheel::OLD_325`      |
| Half-Cut New 3.25" | 3.246           | `lemlib::Omniwheel::NEW_325_HALF` |
| Half-Cut Old 3.25" | 3.246           | `lemlib::Omniwheel::OLD_325_HALF` |
| New 4"             | 4.00            | `lemlib::Omniwheel::NEW_4`        |
| Old 4"             | 4.18            | `lemlib::Omniwheel::OLD_4`        |
| Half-Cut New 4"    | 3.995           | `lemlib::Omniwheel::NEW_4_HALF`   |
| Half-Cut Old 4"    | 4.175           | `lemlib::Omniwheel::OLD_4_HALF`   |

Record the wheel diameter, we'll need it soon.

### Horizontal Drift

 [//]: # (TODO: Link tuning tutorial here) 
Don't worry about horizontal drift, we'll cover it in the tuning tutorial. For now, just set it to 2.

#### Code Config

We now have all the necessary information to configure the drivetrain. We will use the `Drivetrain` class to store this info, see the example below:

```{important}
This needs to be created after the motors and motor groups have been created, and has to be outside of a function
```

```cpp
// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motor_group, // left motor group
                              &right_motor_group, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_4, // using new 4" omnis
                              360, // drivetrain rpm is 360
                              2 // horizontal drift is 2 (for now)
);
```
## Odometry

Odometry is the algorithm that tracks the robots position. It does this through internal motors encoders (IME) and/or tracking wheels and/or V5 Inertial Sensors (IMU). We need to configure these sensors so LemLib can interact with them

### IMU

For the IMU, all we need to do is find the port it is connected to. Enter the devices menu on the brain screen, and see what port the IMU is connected to. Record this, we will need it soon.

### Tracking Wheels

Tracking wheels are independent wheels that have an encoder attached to them. See the image below:

![Tracking Wheel](../assets/2_configuration/tracking_wheel.png)

LemLib can work on any tracking setup, but some setups perform much better than others. See the table below:

##### Heading Tracking:

| Recommended | Acceptable                  | Not Recommended   |
| ----------- | --------------------------- | ----------------- |
| 1x IMU      | 2x parallel tracking wheels | IMEs              |

##### Lateral Position Tracking

There are more possible configs for lateral position tracking. Let's start with the recommended vertical tracking:

| Recommended                | Acceptable |
| -------------------------- | ---------- |
| 1x vertical tracking wheel | IMEs       |

Recommended horizontal tracking:

| Recommended                  | Acceptable      | Not Recommended           |
| ---------------------------- | --------------- | ------------------------- |
| 1x horizontal tracking wheel | traction wheels | Omniwheel-only drivetrain |

It's recommended to use a horizontal tracking wheel even you have traction wheels, since even with traction wheels the robot can still slide horizontally slightly. It is strongly recommended to use a horizontal tracking wheel if your drivetrain only uses omniwheels, since the robot can drift horizontally a lot.

```{seealso}
The optimal tracking setup is 1x IMU, 1x vertical tracking wheel, 1x horizontal tracking wheel
```
#### Code Config

##### Tracking Wheels

Tracking wheels can use either an [Optical Shaft Encoder](https://www.vexrobotics.com/276-2156.html), or a [V5 Rotation Sensor](https://www.vexrobotics.com/276-6050.html). In our testing, there was no difference in accuracy. Use whatever is most convenient.

Tracking wheels require encoders, and encoders have 2 properties we need to know: the port(s) they are connected to, and whether they should be reversed or not.

First, we need to create the encoders. The process is different for the 2 different sensors:

##### Optical Shaft Encoder

```{important}
The optical shat encoder uses 2 ADI (tri-port) ports. However, there are only a few valid port combinations, they are as follows:
('A', 'B'); ('C', 'D'); ('E', 'F'); ('G', 'H')
```

```{seealso}
[pros::ADIEncoder documentation](https://pros.cs.purdue.edu/v5/api/cpp/adi.html#pros-adiencoder)
```


```cpp
// create an optical shaft encoder connected to ports 'A' and 'B'
pros::ADIEncoder encoder('A', 'B');
```
