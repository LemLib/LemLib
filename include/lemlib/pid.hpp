#pragma once

#include "units/units.hpp"
#include <optional>

namespace lemlib {
struct Gains {
        double kP = 0;
        double kI = 0;
        double kD = 0;
};

class PID {
    public:
        PID(double kP, double kI, double kD, double windupRange = 0, bool signFlipReset = false);

        PID(Gains gains, double windupRange = 0, bool signFlipReset = false);

        Gains getGains();

        void setGains(Gains gains);

        double update(double error);

        void reset();

        void setSignFlipReset(bool signFlipReset);
        void setWindupRange(double windupRange);
    private:
        Gains m_gains;

        bool signFlipReset;
        double windupRange;

        double previousError = 0;
        double integral = 0;
};

class ExitCondition {
    public:
        ExitCondition(const double range, const Time time);

        bool getExit();

        bool update(const double input);

        void reset();
    private:
        std::optional<Time> startTime;
        const double range;
        bool done = false;
        const Time time;
};
} // namespace lemlib