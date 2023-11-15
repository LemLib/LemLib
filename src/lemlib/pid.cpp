/**
 * @file src/lemlib/pid.cpp
 * @author LemLib Team
 * @brief FAPID class member definitions
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <math.h>
#include "lemlib/pid.hpp"

namespace lemlib {
// define static variables
template<isQuantity T> std::string FAPID<T>::input = "FAPID"; //todo: needs testing
template<isQuantity T> pros::Task* FAPID<T>::logTask = nullptr;
template<isQuantity T> pros::Mutex FAPID<T>::logMutex = pros::Mutex();

}; // namespace lemlib