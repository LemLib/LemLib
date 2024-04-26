#pragma once

#include "pros/rtos.hpp"
#include <memory>
#include <vector>

namespace lemlib {
/**
 * @brief Abstract exit condition. Intended for use only by advanced users / internally.
 */
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

/**
 * @brief Exit condition that exits when input has been in `range` for longer than `time` ms
 *
 * @warning Error exit conditions are only intended to be used internally and by advanced users.
 * Most of the time the user should only need to interact with `ErrorExitCondition::Config`
 */
class ErrorExitCondition : public ExitCondition {
    public:
        /**
         * @brief Configuration for an `ErrorExitCondition`
         *
         * @see ErrorExitCondition
         */
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
 * @brief Exits if any of it's children are currently in an exitted state. Intended for use only by advanced users /
 * internally.
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

/**
 * @brief Abstract factory for creating an `ExitCondition`. Intended for use only by advanced users / internally.
 */
class ExitConditionFactory {
    public:
        virtual std::unique_ptr<ExitCondition> create() const = 0;
};

/**
 * @brief Used by user to configure multiple ErrorExitConditions whilst ensuring atomicity / thread-safety.
 * Used by the chassis to create exit conditions for motions.
 *
 * Intended for use by slightly advanced users
 */
class ErrorExitConditionGroupFactory : public ExitConditionFactory {
    public:
        ErrorExitConditionGroupFactory(std::vector<ErrorExitCondition::Config> configs);

        /** @brief removes all configs */
        void clear();

        /** @brief adds configs to the end of the config vector */
        void push_back(ErrorExitCondition::Config& newConfigs...);

        /** @brief creates a new ExitConditionGroup with the current configs */
        std::unique_ptr<ExitCondition> create() const override;

        /** @brief modifies the configs whilst ensuring atomicity */
        void setConfigs(std::vector<ErrorExitCondition::Config>& newConfig);

        /** @brief reads the configs whilst ensuring atomicity */
        std::vector<ErrorExitCondition::Config> getConfigs() const;
    private:
        /** @brief used to ensure atomicity */
        mutable pros::Mutex mutex;
        /** @brief used to create ErrorExitConditions by `ErrorExitConditionGroupFactory::create()` */
        std::vector<ErrorExitCondition::Config> configs;
};
} // namespace lemlib