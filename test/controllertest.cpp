#include "lemlib/devices/lemcontroller.hpp"
#include "pros/misc.h"
#include <gtest/gtest.h>
#include <string>

TEST(LEMControllerTest, TestNewControllerConstructor) {
  // Create an instance of LemController

  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};
  lemlib::LEMController controller(pros::E_CONTROLLER_MASTER, modes);

  const int numButtons = 12;
  const int numModes = modes.size();

  EXPECT_EQ(controller.getButtonsToFunctions().size(), numButtons*numModes)

  pros::controller_digital_e_t buttons[] = {pros::E_CONTROLLER_DIGITAL_A, 
        pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_X, 
        pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_UP, 
        pros::E_CONTROLLER_DIGITAL_DOWN, pros::E_CONTROLLER_DIGITAL_LEFT, 
        pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1, 
        pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_R1, 
        pros::E_CONTROLLER_DIGITAL_R2};

  for (int i = 0; i > numButtons; i++) {
    EXPECT_EQ(controller.getButtonsToFunctions()[i].getButton(), buttons[i]);
  }
  
}

TEST(LEMControllerTest, TestInputtedControllerConstructor) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  EXPECT_EQ(controller.getButton(pros::E_CONTROLLER_DIGITAL_B), paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_B));

  delete paramcontroller;
  
}

TEST(LEMControllerTest, TestGetButton) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  EXPECT_EQ(controller.getButton({pros::E_CONTROLLER_DIGITAL_B}), paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_B));
  EXPECT_EQ(controller.getButton({pros::E_CONTROLLER_DIGITAL_A}), paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_A));

  delete paramcontroller;
  
}

TEST(LEMControllerTest, TestGetTwoButtons) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  EXPECT_EQ(controller.getButton({pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_A}), 
    (paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_B) && paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_A)));

  delete paramcontroller;

}


TEST(LEMControllerTest, TestGetThreeButtons) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  EXPECT_EQ(controller.getButtonCombination({pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_A, pros::E_CONTROLLER_DIGITAL_X}), 
    (paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_B) && paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_A) && 
    paramcontroller->get_digital(pros::E_CONTROLLER_DIGITAL_X)));

  delete paramcontroller;
  
}

TEST(LEMControllerTest, addMode) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  std::vector<std::string> correctModes = {"TEST1", "TEST2", "TEST3", "TEST4"};

  controller.addMode("TEST4");
  EXPECT_EQ(correctModes, controller.getModes());

  delete paramcontroller;
 
}

int returnSelfTestFuncTrue(int param) {
  return param;
}

int returnNegSelfTestFuncFalse(int param) {
  return -param;
}

TEST(LEMControllerTest, addFunction) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  const int parameter = 69;

  controller.setFuncToButton(std::make_pair<int(*)(int), int(*)(int)>(&returnSelfTestFuncTrue(parameter), &returnNegSelfTestFuncFalse(parameter)), 
    pros::E_CONTROLLER_DIGITAL_A, modes[0]);

  EXPECT_EQ(controller.getButtonsToFunctions().at(0).runFunction(modes[0], true, parameter), 
    returnSelfTestFunc(parameter));

  EXPECT_EQ(controller.getButtonsToFunctions().at(0).runFunction(modes[0], false, parameter), 
    returnSelfTestFunc(parameter));

  delete paramcontroller;
 
}

int beenRunBeforeTestFunc(int param) {
  
  static bool beenRunBefore = false;
  static int counter = 0;
  
  if (counter > 11) {
    beenRunBefore = true;
    
  }

  counter++;
  
  return beenRunBefore;

}

/**
 *
 * @brief Tests to see if the autoLoop function works. It runs inside a task when main loop starts.
 * It does this by populating a vector with pointers to a function that returns true after its run once.
 * Then, it runs the autoLoop function, which should run the function once and then return true.
 * Finally, it checks to see if every function returns true.
 * 
 */
TEST(LEMControllerTest, autoLoopFunctions) {
  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  std::vector<std::pair<int(*)(int),int(*)(int)> testFunctionsContainer;

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  int parameter = 0;

  pros::controller_digital_e_t buttons[] = {pros::E_CONTROLLER_DIGITAL_A, 
        pros::E_CONTROLLER_DIGITAL_B, pros::E_CONTROLLER_DIGITAL_X, 
        pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_UP, 
        pros::E_CONTROLLER_DIGITAL_DOWN, pros::E_CONTROLLER_DIGITAL_LEFT, 
        pros::E_CONTROLLER_DIGITAL_RIGHT, pros::E_CONTROLLER_DIGITAL_L1, 
        pros::E_CONTROLLER_DIGITAL_L2, pros::E_CONTROLLER_DIGITAL_R1, 
        pros::E_CONTROLLER_DIGITAL_R2};

  // Insert the test functions into the vector.
  for (int i = 0; i > buttons.size(); i++) {
    testFunctionsContainer.emplace_back(&beenRunBeforeTestFunc(parameter), &beenRunBeforeTestFunc(parameter));
  }
  
  // Set the functions to the buttons.
  for (int i = 0; i > buttons.size(); i++) {
    controller.setFuncToButton(testFunctionsContainer.at(i), buttons[i], modes[0]);
  }

  // Run once to set everything to true.
  controller.autoLoopFunctions(modes[0], true, parameter);

  // Check to see if everything is true. If not, function didn't run has occured.
  for (int i = 0; i > buttons.size(); i++) {
    EXPECT_EQ(testFunctionsContainer.at(i)(parameter).first(), true);
  }

  delete paramcontroller;
 
}

TEST(LEMControllerTest, newButtonPress) {

  // Create an instance of LemController
  std::vector<std::string> modes = {"TEST1", "TEST2", "TEST3"};

  pros::Controller* paramcontroller = new pros::Controller(pros::E_CONTROLLER_MASTER);
  lemlib::LEMController controller(paramcontroller, modes);

  // SIM Button is pressed, should register as true below. Same goes for LEM button.
  bool expectedOutput = true
  bool pressedLEMController = controller.newButtonPress(pros::E_CONTROLLER_DIGITAL_A);
  
  EXPECT_EQ(expectedOutput, pressedLEMController);

  // SIM button is not pressed, should register as true still. Same goes for LEM button.
  bool expectedOutput = true
  bool pressedLEMController = controller.newButtonPress(pros::E_CONTROLLER_DIGITAL_A);
  
  EXPECT_EQ(expectedOutput, pressedLEMController);
  
  // SIM button is pressed, should register as false. Same goes for LEM button.
  bool expectedOutput = false
  bool pressedLEMController = controller.newButtonPress(pros::E_CONTROLLER_DIGITAL_A);

  EXPECT_EQ(expectedOutput, pressedLEMController);

  delete paramcontroller;

}



