#pragma once

#include "units/units.hpp"
#include "pros/rtos.hpp"

namespace lemlib {
struct Gains {
        float kP = 0;
        float kI = 0;
        float kD = 0;
};

template <isQuantity Q> class PID {
    public:
        PID(float kP, float kI, float kD, Q windupRange = Q(0), bool signFlipReset = false)
            : gains({kP, kI, kD}),
              windupRange(windupRange),
              signFlipReset(signFlipReset) {}

        PID(Gains gains, Q windupRange = Q(0), bool signFlipReset = false)
            : gains(gains),
              windupRange(windupRange),
              signFlipReset(signFlipReset) {}

        Gains getGains() { return gains; }

        void setGains(Gains gains) { this->gains = gains; }

        float update(Q error) {
            this->integral += error;
            if (units::sgn(error) != units::sgn((this->previousError)) && this->signFlipReset) this->integral = Q(0);
            if (units::abs(error) > this->windupRange && this->windupRange != 0) this->integral = Q(0);

            const float derivative = error - this->previousError;
            this->previousError = error;

            return error * this->gains.kP + this->integral * this->gains.kI + derivative * this->gains.kD;
        }

        void reset() {
            this->previousError = Q(0);
            this->integral = Q(0);
        }
    private:
        Gains gains;

        const bool signFlipReset;
        const Q windupRange;

        Q previousError = Q(0);
        Q integral = Q(0);
};

template <isQuantity Q> class ExitCondition {
    public:
        ExitCondition(const Q range, const Time time)
            : range(range),
              time(time) {}

        bool getExit() { return done; }

        bool update(const Q input) {
            const Time currentTime = pros::millis() * msec;
            if (units::abs(input) > this->range) this->startTime = -1 * sec;
            else if (this->startTime == -1 * sec) this->startTime = currentTime;
            else if (currentTime >= this->startTime + this->time) this->done = true;

            return this->done;
        }

        void reset() {
            this->startTime = -1 * sec;
            this->done = false;
        }
    private:
        Time startTime = -1 * sec;
        bool done = false;
        const Time time;
        const Q range;
};
} // namespace lemlib