# LemLib
Welcome to LemLib! This open-source PROS template aims to introduce common algorithms like Pure Pursuit and Odometry for new and experienced teams alike.

The creation of this template was inspired by [EZ-Template](https://github.com/EZ-Robotics/EZ-Template) and [OkapiLib](https://github.com/OkapiLib/OkapiLib). We aim to develop a library that strikes a balance between ease-of-use, akin to that of EZ-Template, and comprehensive functionality, similar to that of OkapiLib.

> Want a place to chat with the devs and other users? Join our [discord server](https://discord.gg/pCHr7XZUTj).

## License
This project is licensed under the MIT license. Check [LICENSE](https://github.com/LemLib/LemLib/blob/master/LICENSE) for more details.

## Features
- Generic PID class
- Odometry
- Odom turn to face point
- [Boomerang controller](https://www.desmos.com/calculator/sptjw5szex)
- Pure Pursuit
- [Path Generator](https://github.com/LemLib/Path-Gen) support

## Example Project
You can find a fully annotated example project [here](https://github.com/LemLib/LemLib/blob/master/src/main.cpp)

## Tutorials
The tutorials provide a detailed walkthrough through all the features of LemLib. It covers everything from installation to pure pursuit.
 - 1: [Getting Started](https://lemlib.github.io/LemLib/md_docs_tutorials_1_getting_started.html)
 - 2: [Setting up the Chassis](https://lemlib.github.io/LemLib/md_docs_tutorials_2_setting_up_the_chassis.html)
 - 3: [Driver Control](https://lemlib.github.io/LemLib/md_docs_tutorials_3_driver_control.html)
 - 4: [Tuning and Moving](https://lemlib.github.io/LemLib/md_docs_tutorials_4_auto_and_tuning.html)
 - 5: [Pure Pursuit](https://lemlib.github.io/LemLib/md_docs_tutorials_5_pure_pursuit.html)

## FAQ
_**1. Help! why is my controller vibrating?**_
Your inertial sensor calibration failed.
Check if its connected to the right port and try again.

_**2. What drivetrains are supported?**_
Only tank/differential.
This is not going to change, as other drivetrains simply aren't popular enough.

_**3. Do I need tracking wheels?**_
No, but it is recommended.
You should absolutely have a horizontal tracking wheel if you don't have traction wheels, and you have to spend extra effort tuning your movements to prevent any wheel slip.

_**4. Do I need an inertial sensor?**_
No, but it is highly recommended.
The one exception to this would be if you have 2 parallel tracking wheels which are tuned well and are perfectly square. LemLib will work without it, but the accuracy will be compromised. 

_**5. Do I need an SD card?**_
Only if you use Pure Pursuit `chassis.follow()`.
This will change in the next major release however. In fact you could try it right now in our latest beta

_**6. What are the units?**_
Lemlib uses a custom units system that lets you can specify or read any quantity in any units you want, and does the conversion automatically.

_**7. Is LemLib VRC legal?**_
Yes.
Per the recf student-centred policy, in the context of third-party libraries
> Students should be able to understand and explain the code used on their robots

In other words, you need to know how LemLib works. You don't need to know the details like all the math, just more or less how the algorithm works. If you want to learn more about LemLib, you can look through the documentation and ask questions on our Discord server

## Documentation
Check out the [Doxygen documentation](https://lemlib.github.io/LemLib/index.html)

## Contributing
Want to contribute? Please read [CONTRIBUTING.md](https://github.com/LemLib/LemLib/blob/master/.github/CONTRIBUTING.md) and join our [discord server](https://discord.gg/pCHr7XZUTj)

## Code of Conduct
See the [Code of Conduct](https://github.com/LemLib/LemLib/blob/master/.github/CODE_OF_CONDUCT.md) on how to behave like an adult.
