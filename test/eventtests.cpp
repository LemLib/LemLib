#include "lemlib/eventhandler/eventhandler.hpp"
#include "lemlib/eventhandler/testevents.hpp"
#include "pros/misc.h"
#include <gtest/gtest.h>
#include <string>
/*
TEST(EventHandlerTest, DuplicateIDTest) {
    lemlib::TESTEvent testEventA(false, 0);
    lemlib::TESTEvent testEventB(false, 0);
    lemlib::TESTEvent testEventC(true, 0);
    lemlib::TESTEvent testEventD(false, 0);
    lemlib::TESTEvent testEventE(true, 0);

    std::vector<std::shared_ptr<lemlib::Event>> testEvents(
        {std::make_shared<lemlib::TESTEvent>(testEventA), std::make_shared<lemlib::TESTEvent>(testEventB),
         std::make_shared<lemlib::TESTEvent>(testEventC), std::make_shared<lemlib::TESTEvent>(testEventD),
         std::make_shared<lemlib::TESTEvent>(testEventE)});

    bool foundDuplicates = false;

    try {
        lemlib::EventHandler testEventHandler(testEvents);
        std::cout << "Failed to identify " << std::endl;
        foundDuplicates = false;
    } catch (std::exception& e) {
        std::cout << "Properly Handled Duplicate ID" << std::endl;
        foundDuplicates = true;
    }

    EXPECT_EQ(foundDuplicates, true);
}

TEST(EventHandlerTest, TestEventCheck) {
    lemlib::TESTEvent testEventA(false, 0);
    lemlib::TESTEvent testEventB(false, 1);
    lemlib::TESTEvent testEventC(true, 2);
    lemlib::TESTEvent testEventD(false, 3);
    lemlib::TESTEvent testEventE(true, 4);

    std::vector<std::shared_ptr<lemlib::Event>> testEvents(
        {std::make_shared<lemlib::TESTEvent>(testEventA), std::make_shared<lemlib::TESTEvent>(testEventB),
         std::make_shared<lemlib::TESTEvent>(testEventC), std::make_shared<lemlib::TESTEvent>(testEventD),
         std::make_shared<lemlib::TESTEvent>(testEventE)});

    lemlib::EventHandler testEventHandler(testEvents);

    bool properOutputA = false;
    bool properOutputB = false;
    bool properOutputC = false;
    bool properOutputD = false;
    bool properOutputE = false;

    if (testEventHandler.checkEvent(0) == false) {
        std::cout << " Event A triggered. " << std::endl;
        properOutputA = true;
    }

    if (testEventHandler.checkEvent(1) == false) {
        std::cout << " Event B triggered. " << std::endl;
        properOutputB = true;
    }

    if (testEventHandler.checkEvent(2) == true) {
        std::cout << " Event C triggered. " << std::endl;
        properOutputC = true;
    }

    if (testEventHandler.checkEvent(3) == false) {
        std::cout << " Event D triggered. " << std::endl;
        properOutputD = true;
    }

    if (testEventHandler.checkEvent(4) == false) {
        std::cout << " Event E triggered. " << std::endl;
        properOutputE = true;
    }
}
*/