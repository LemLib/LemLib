#include <cmath>
#include <memory>
#include <mutex>
#include <vector>
#include "lemlib/exitcondition.hpp"

namespace lemlib {

/**
 * @brief Create a new Exit Condition
 *
 * @param range the range where the countdown is allowed to start
 * @param time how much time to wait while in range before exiting
 */
ErrorExitCondition::ErrorExitCondition(const float range, const int time)
    : ErrorExitCondition(ErrorExitCondition::Config {.range = range, .time = time}) {}

ErrorExitCondition::ErrorExitCondition(ErrorExitCondition::Config config)
    : config(config) {}

/**
 * @brief whether the exit condition has been met
 *
 * @return true exit condition met
 * @return false exit condition not met
 */
bool ErrorExitCondition::getExit() const { return this->done; }

/**
 * @brief update the exit condition
 *
 * @param input the input for the exit condition
 * @return true exit condition met
 * @return false exit condition not met
 */
bool ErrorExitCondition::update(const float input) {
    const int curTime = pros::millis();
    if (std::fabs(input) > this->config.range) this->startTime = -1;
    else if (this->startTime == -1) this->startTime = curTime;
    else if (curTime >= this->startTime + this->config.time) this->done = true;
    return this->done;
}

ErrorExitConditionGroupFactory::ErrorExitConditionGroupFactory(std::vector<ErrorExitCondition::Config> configs)
    : configs(configs) {}

std::unique_ptr<ExitCondition> ErrorExitConditionGroupFactory::create() const {
    std::vector<std::shared_ptr<ExitCondition>> conditions;
    for (const auto& config : this->getConfigs()) conditions.push_back(std::make_shared<ErrorExitCondition>(config));
    return std::make_unique<ExitConditionGroup>(conditions);
}

std::vector<ErrorExitCondition::Config> ErrorExitConditionGroupFactory::getConfigs() const {
    std::lock_guard guard(this->mutex);
    return this->configs;
}

void ErrorExitConditionGroupFactory::setConfigs(std::vector<ErrorExitCondition::Config>& newConfigs) {
    std::lock_guard guard(this->mutex);
    this->configs = newConfigs;
}

void ErrorExitConditionGroupFactory::push_back(ErrorExitCondition::Config& newConfigs...) {
    auto modifiedConfigs = this->getConfigs();
    modifiedConfigs.push_back(newConfigs);
    this->setConfigs(modifiedConfigs);
}

void ErrorExitConditionGroupFactory::clear() {
    auto newConfigs = this->getConfigs();
    newConfigs.clear();
    this->setConfigs(newConfigs);
}

ExitConditionGroup::ExitConditionGroup(std::vector<std::shared_ptr<ExitCondition>> children) {
    children.swap(this->children);
}

bool ExitConditionGroup::update(const float input) {
    for (auto child : this->children) {
        if (child->update(input)) return true;
    }
    return false;
}

bool ExitConditionGroup::getExit() const {
    for (auto child : this->children) {
        if (child->getExit()) return true;
    }
    return false;
}
} // namespace lemlib