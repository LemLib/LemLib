
#include "lemlib/movements/accuracysim.hpp"

namespace lemlib {

AccuracySIM::AccuracySIM() {
    controller = new LEMController();
    autonSelector = new AutonSelector();
    inputReader = new InputReader();
    logger = new Logger();
    poller = new Poller();
}

AccuracySIM::AccuracySIM(LEMController* controllerParam, AutonSelector* autonSelectorParam,
                         InputReader* inputReaderParam, Logger* loggerParam, Poller* pollerParam) {
    controller = controllerParam;
    autonSelector = autonSelectorParam;
    inputReader = inputReaderParam;
    logger = loggerParam;
    poller = pollerParam;
}

AccuracySIM::~AccuracySIM() {
    delete controller;
    delete autonSelector;
    delete inputReader;
    delete logger;
    delete poller;
}

void AccuracySIM::countdown() {
    controller->rumble(". . ."); // Rumble the controller to let the driver know that the simulation is starting
}

void AccuracySIM::markTime(int circleIndex) {
    times.emplace_back(pros::millis() - lastTimeEnteredCircle);
    lastTimeEnteredCircle = pros::millis();
}

void AccuracySIM::saveData() { logger.saveData(); }

void AccuracySIM::recordMaxDistance() {
    static float maxDistance = 0;

    float xcirclecord = circles.at(0).getX();
    float ycirclecord = circles.at(0).getY();

    // Distance formula
    float actualCurrentDistance =
        sqrt(pow(xcirclecord - botDimensions.getX(), 2) + pow(ycirclecord - botDimensions.getY(), 2));

    if (actualCurrentDistance > maxDistance) { maxDistance = actualCurrentDistance; }

    maxDistances.at(times.size() - 1) = maxDistance;
}

void AccuracySIM::runSimulation() {
    countdown();

    // Start main loop.
    pros::Task task {[=] {
        while (bot is not at the last circle) {
            recordMaxDistance();

            if (bot is at center of a circle) {
                markTime(index of circle);
                maxDistances.emplace_back(0);
            }

            // If up, down, X, and Y are pressed, reset.
            if (controller->getButtonCombination(pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_Y,
                                                 pros::E_CONTROLLER_DIGITAL_UP, pros::E_CONTROLLER_DIGITAL_DOWN)) {
                restart();
            }

            pros::delay(20);
        }

        saveData();
    }};
}

void AccuracySIM::restart() {
    times.clear();
    maxDistances.clear();
    lastTimeEnteredCircle = 0;
    runSimulation();
}

std::pair<float, float> AccuracySIM::getPercentagePerformance() {
    std::pair<float, float> performance;

    return performance;
}

} // namespace lemlib