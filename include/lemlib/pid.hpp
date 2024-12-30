#pragma once

#include "units/units.hpp"

namespace lemlib {
struct Gains {
        float kF = 0;
        float kA = 0;
        float kP = 0;
        float kI = 0;
        float kD = 0;
};

template <isQuantity Q> class FAPID {
    public:
        FAPID(float kF, float kA, float kP, float kI, float kD, Q windupRange = Q(0), bool signFlipReset = false);
        FAPID(Gains gains, Q windupRange = Q(0), bool signFlipReset = false);
    private:
        Gains gains;

        const bool signFlipReset;
        const Q windupRange;

        Q previousError = Q(0);
        Q integral = Q(0);
};

template <isQuantity Q> class ExitCondition {
    public:
        ExitCondition(const Q range, const Time time);
        bool getExit();
        bool update(const Q input);
        void reset();
    private:
        Time startTime = -1 * sec;
        bool done = false;
        const Time time;
        const Q range;
};
} // namespace lemlib