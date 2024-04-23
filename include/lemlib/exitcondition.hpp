#pragma once

#include "pros/rtos.hpp"
#include <memory>
#include <vector>

namespace lemlib {
class ExitCondition {
    public:
        /**
         * @brief whether the exit condition has been met
         *
         * @return true exit condition met
         * @return false exit condition not met
         */
        virtual bool getExit() const = 0;
        /**
         * @brief update the exit condition
         *
         * @param input the input for the exit condition
         * @return true exit condition met
         * @return false exit condition not met
         */
        virtual bool update(const float input) = 0;
};

class ErrorExitCondition : public ExitCondition {
    public:
        struct Config {
                /** @brief the range where the countdown is allowed to start */
                float range;
                /** @brief how much time to wait while in range before exiting */
                int time;
        };

        /**
         * @brief Create a new Exit Condition
         *
         * @param range the range where the countdown is allowed to start
         * @param time how much time to wait while in range before exiting
         */
        ErrorExitCondition(const float range, const int time);
        /**
         * @brief Create a new Exit Condition
         */
        ErrorExitCondition(ErrorExitCondition::Config config);

        /**
         * @brief whether the exit condition has been met
         *
         * @return true exit condition met
         * @return false exit condition not met
         */
        bool getExit() const override;

        /**
         * @brief update the exit condition
         *
         * @param input the input for the exit condition
         * @return true exit condition met
         * @return false exit condition not met
         */
        bool update(const float input) override;
    protected:
        ErrorExitCondition::Config config;
        int startTime = -1;
        bool done = false;
};

/**
 * @brief exits if any of the children exit
 */
class ExitConditionGroup : public ExitCondition {
    public:
        /**
         * @brief Create a new Exit Condition Group
         */
        ExitConditionGroup(std::vector<std::shared_ptr<ExitCondition>> children);

        /**
         * @brief whether the exit condition has been met
         *
         * @return true exit condition met
         * @return false exit condition not met
         */
        bool getExit() const override;

        /**
         * @brief update the exit condition
         *
         * @param input the input for the exit condition
         * @return true exit condition met
         * @return false exit condition not met
         */
        bool update(const float input) override;
    protected:
        std::vector<std::shared_ptr<ExitCondition>> children;
};

class ExitConditionFactory {
    public:
        virtual std::unique_ptr<ExitCondition> create() const = 0;
};

class ErrorExitConditionGroupFactory : public ExitConditionFactory {
    public:
        ErrorExitConditionGroupFactory(std::vector<ErrorExitCondition::Config> configs);
        /** @brief removes all configs */
        void clear();

        /** @brief adds configs to the end of the config vector */
        void push_back(ErrorExitCondition::Config& newConfigs...);

        /** @brief creates a new ExitConditionGroup with the current configs */
        std::unique_ptr<ExitCondition> create() const override;

        void setConfigs(std::vector<ErrorExitCondition::Config>& newConfig);
        std::vector<ErrorExitCondition::Config> getConfigs() const;
    private:
        mutable pros::Mutex mutex;
        std::vector<ErrorExitCondition::Config> configs;
};
} // namespace lemlib