#include <math.h>
#include <vector>
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"
#include "pros/rtos.hpp"
#include "lemlib/smartMotor.hpp"

namespace lemlib {
/**
 * @brief Constructor for ADI Encoder-based SmartMotor.
 */
SmartMotor::SmartMotor(pros::MotorGroup* actuator, pros::adi::Encoder* sensor, PID controller)
    : actuator(actuator), controller(controller), encoder(sensor) {}

/**
 * @brief Constructor for V5 Rotation Sensor-based SmartMotor.
 */
SmartMotor::SmartMotor(pros::MotorGroup* actuator, pros::Rotation* sensor, PID controller)
    : actuator(actuator), controller(controller), rotation(sensor) {}

/**
 * @brief Constructor for IME-based SmartMotor.
 */
SmartMotor::SmartMotor(pros::MotorGroup* actuator, PID controller)
    : actuator(actuator), controller(controller) {
    this->ime = actuator; // Defaults to using integrated motor encoder (IME)
}

/**
 * @brief Resets the absolute rotation/position of the motor.
 * 
 * Depending on the sensor type, this function resets the encoder, V5 rotation sensor, 
 * or IME-based motor group position back to zero.
 */
int SmartMotor::reset() {
    if (this->encoder != nullptr) this->encoder->reset();
    if (this->rotation != nullptr) this->rotation->reset_position();
    if (this->ime != nullptr) this->ime->tare_position_all();
    return 0; // kept for future expandability
}

/**
 * @brief Returns the current position of the motor.
 * 
 * This function reads from the active sensor (ADI Encoder, V5 Rotation Sensor, or IME).
 */
float SmartMotor::getRotation() {
    if (this->encoder != nullptr) 
        return (float(this->encoder->get_value()));
    else if (this->rotation != nullptr) 
        return (float(this->rotation->get_position()));
    else if (this->ime != nullptr) {
        std::vector<double> positions = this->ime->get_position_all();
        std::vector<float> positions_float(positions.size());
        std::transform(positions.begin(), positions.end(), positions_float.begin(), 
               [](double val) { return static_cast<float>(val); });
        return lemlib::avg(positions_float);
    } 
    return 0; // Default return value if no valid sensor is available.
}

/**
 * @brief Moves the motor to a target position using a PID loop.
 * 
 * The motor moves to the target position using PID control. If `async` is true, the function
 * spawns a separate task to run the PID loop asynchronously.
 */
int SmartMotor::movePID(float target, float timeout, float acceptableRange, bool async) {
    if (async) {
        // Start an asynchronous task to run the PID loop.
        pros::Task asynctask([=]() {
            this->movePID(target, timeout, false); // Call the synchronous version
        });
        pros::delay(10); // Delay to give the task time to start
        return 2;                     // Indicate that async task has been started.
    }
    unsigned long start_time = pros::millis();

    // Loop until the target is reached or the timeout occurs
    while (true) {
        // Get the current position from the system
        float current_pos = this->getRotation();
        float error = target - current_pos;

        // If the error is within an acceptable range, exit the loop
        if (fabs(error) < acceptableRange) break;

        // Check if the timeout has expired
        if (pros::millis() - start_time > timeout) break;

        // Update the PID controller with the error to get the control signal
        float control_signal = this->controller.update(error) / 200;

        // Send the control signal to the movement system (motor/actuator)
        this->actuator->move(control_signal);
        
        // Optionally, add a small delay to prevent too frequent updates (e.g., 10ms)
        printf("PID-Signal: %f | Current: %f | Target: %f \n", control_signal, current_pos, target);
        pros::delay(10);  // Adjust as needed based on your system's requirements
    }
    // After the loop ends (either target reached or timeout), stop the motor
    this->actuator->brake();

    // Return success status 
    return (fabs(target - this->getRotation()) < 0.01) ? 1 : 0;  // Return 1 if target reached, 0 if timed out
}
} // namespace lemlib