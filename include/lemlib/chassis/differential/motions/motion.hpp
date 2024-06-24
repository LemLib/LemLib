#pragma once

#include "lemlib/chassis/differential/drivetrain.hpp"

namespace lemlib::differential {
class Motion {
    protected:
        bool m_finished = false;
    public:
        virtual std::unique_ptr<Drivetrain> calculate(const std::unique_ptr<Drivetrain> drivetrain,
                                                      const DrivetrainState& state) = 0;

        bool isFinished() const;
};
} // namespace lemlib::differential