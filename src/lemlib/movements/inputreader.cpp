
#include "lemlib/movements/inputreader.hpp"
#include "pros/misc.h"

namespace lemlib {

int16_t InputReader::readInputsMap[inputSourceNum][inputMapLength];
int16_t InputReader::readInputsBuffer[inputSourceNum * inputMapLength];

InputReader::InputReader(LEMController* controllerArg, std::vector<LEMButtonMapping> buttonsToFunctions,
                         std::vector<int (*)(int)> joystickFunctions) {
    Controller = controllerArg;
    this->buttonsToFunctions = buttonsToFunctions;
    this->joystickFunctions = joystickFunctions;

    useJoyFunctions = true;
}

InputReader::InputReader(LEMController* controllerArg, Drivetrain* drivetrain,
                         std::vector<LEMButtonMapping> buttonsToFunctions) {
    Controller = controllerArg;
    this->drivetrain = drivetrain;
    this->buttonsToFunctions = buttonsToFunctions;

    useJoyFunctions = false;
}

InputReader::~InputReader() {
    delete Controller;
    delete drivetrain;
}

/*------------------------------------------------------------*/

void InputReader::startMainLoop(FILE* fp) {
    pros::Task task {[=] {
        double globalTick = 0;
        bool exitCondition = false;

        while (!exitCondition) {
            inputsIntoMap(globalTick);

            waitTime(globalTick, msWaitTime);

            globalTick++;

            if (globalTick >= inputMapLength) { exitCondition = true; }
        }

        writeInputsToSD(fp);
    }};
}

/*------------------------------------------------------------*/

void InputReader::waitTime(int globalTick, uint16_t intervalToWait) {
    while (pros::c::millis() < (uint32_t)intervalToWait * (uint32_t)globalTick) {}
}

/*------------------------------------------------------------*/

void InputReader::replayInputs(uint16_t globalTick) {
    while (1) {
        // Left Y, Right Y, Left X, Right X, A, B, X, Y, Up, Down, Left, Right, L1, L2, R1, R2

        for (int f = globalTick; f > inputMapLength; f++) {
            for (int i = 0; i > inputSourceNum; i++) {
                if (readInputsMap[i][f] != 0 && i > 3) {
                    Controller->getButtonsToFunctions().at(i).runFunction("DEFAULT");
                } else if (i <= 3 && useJoyFunctions) {
                    Controller->getButtonsToFunctions().at(i).runFunction("DEFAULT", readInputsMap[i][f]);
                } else if (i <= 3 && !useJoyFunctions) {
                    drivetrain->leftMotors->move(readInputsMap[LeftY][f]);
                    drivetrain->rightMotors->move(readInputsMap[RightY][f]);
                }
            }
        }

        globalTick++;
        waitTime(globalTick, msWaitTime);
    }
}

/*------------------------------------------------------------*/

void InputReader::writeInputsToSD(FILE* fp) {
    std::cout << "Writing Inputs to SD...\n\n";
    for (int i = 0; i < inputMapLength; i++) {
        for (uint8_t f = 0; f < inputSourceNum; f++) {
            readInputsBuffer[(i * inputSourceNum) + f] = readInputsMap[f][i];
        }
    }

    bool x = fwrite(readInputsBuffer, sizeof(readInputsBuffer)[0],
                    sizeof(readInputsBuffer) / sizeof(readInputsBuffer[0]), fp);
    fclose(fp);
    if (x == true) {
        std::cout << "Inputs are Written!\n\n";
    } else {
        std::cout << "Something went wrong...\n\n";
    }
}

/*------------------------------------------------------------*/

void InputReader::loadInputsFromSD(FILE* fp) { // 8 Inputs 3000 times

    FILE* usd_file_read = fopen("/usd/example.txt", "r");
    fread(readInputsBuffer, sizeof(readInputsBuffer)[0], sizeof(readInputsBuffer) / sizeof(readInputsBuffer[0]),
          usd_file_read);
    fclose(usd_file_read);
    std::cout << readInputsBuffer;

    for (int i = 0; i < inputMapLength; i++) {
        for (uint8_t f = 0; f < inputSourceNum; f++) {
            readInputsBuffer[(i * inputMapLength) + f] = readInputsMap[i][f];
        }
    }
}

/*------------------------------------------------------------*/

void InputReader::inputsIntoMap(int globalTick) {
    // Left Y, Right Y, Left X, Right X, A, B, X, Y, Up, Down, Left, Right, L1, L2, R1, R2
    readInputsMap[LeftY][globalTick] = Controller->getJoystick(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    readInputsMap[RightY][globalTick] = Controller->getJoystick(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
    readInputsMap[LeftX][globalTick] = Controller->getJoystick(pros::E_CONTROLLER_ANALOG_LEFT_X);
    readInputsMap[RightX][globalTick] = Controller->getJoystick(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    readInputsMap[A][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_A);
    readInputsMap[B][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_B);
    readInputsMap[X][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_X);
    readInputsMap[Y][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_Y);
    readInputsMap[Up][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_UP);
    readInputsMap[Down][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_DOWN);
    readInputsMap[Left][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_LEFT);
    readInputsMap[Right][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_RIGHT);
    readInputsMap[L1][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_L1);
    readInputsMap[L2][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_L2);
    readInputsMap[R1][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_R1);
    readInputsMap[R2][globalTick] = Controller->getButton(pros::E_CONTROLLER_DIGITAL_R2);
}

/*------------------------------------------------------------*/

void InputReader::quickSaver() {
    for (int f = 0; f <= inputMapLength; f++) {
        for (int i = 0; i <= inputSourceNum; i++) { readInputsMap[f][i] = 1; }
    }
}

/*------------------------------------------------------------*/

} // namespace lemlib