# Macros

Macros are a way to define a function that gets automatically called when a sequence of buttons are fired.

LemLib has a very simple way of tackling macros. Simply define a function with a `lemlib::ControllerSequence` and a callback function.

Simple macro to toggle an intake:
```cpp
void opcontrol() {
	// create a new macro
	// it runs the intake when the Controller L2 button is pressed, and stops it when it is released
	lemlib::macro::Macro intakeMacro = lemlib::macro::Macro(
		{pros::E_CONTROLLER_DIGITAL_L2}, // the sequence of buttons to press, in this case, only L2
		[]() { intake.move_velocity(200); }, // the function to run when the sequence is pressed (starts the intake)
		[]() { if (!intake.is_stopped()) intake.move_velocity(0); } // the function to run when the sequence is released (stops the intake)
	);

	// create a new macro manager and add the macro to it
	lemlib::macro::MacroManager macros({intakeMacro});

	// the main controller
	pros::Controller controller(pros::E_CONTROLLER_MASTER);

	while (true) {
		// update the macro manager
		macros.check(controller);
		pros::delay(10);
	}
}
```

### Seperate threads

Macro callback functions can be ran in a different task if you want to prevent the main thread from being blocked. This is useful if you want to run a macro that takes a long time to complete, such as a macro that runs a path.

```cpp
lemlib::macro::Macro myMacro = lemlib::macro::Macro(
    // ...
).setThreaded(true); // set the macro to run in a different task
```
