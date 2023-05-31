//
// Created by Rocky Chen on 2023/5/31.
//

#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "pros/vision.hpp"

namespace lemlib {
class Vision {
    public:
        Vision() = delete;
        Vision(std::uint8_t port);
        Vision(const pros::Vision&);
        void setSignature(std::vector<pros::vision_signature_s_t> signatures);
        void addSignature(pros::vision_signature_s_t signature);
        float getDirection();
    private:
        std::unique_ptr<pros::Vision> vision;
        std::vector<pros::vision_signature_s_t> signatures;
}; // class Vision
}; // namespace lemlib