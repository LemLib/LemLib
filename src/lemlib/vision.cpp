//
// Created by Rocky Chen on 2023/5/31.
//

#include "vision.hpp"
#include "pros/vision.hpp"
#include "lemlib/util.hpp"
#include <memory>
#include <cmath>

namespace lemlib {

Vision::Vision(std::uint8_t port) { this->vision = std::make_unique<pros::Vision>(pros::Vision(port)); }

Vision::Vision(const pros::Vision& vision) { this->vision = std::make_unique<pros::Vision>(vision); }

void Vision::addSignature(pros::vision_signature_s_t signature) { this->signatures.push_back(signature); }

void Vision::setSignature(std::vector<pros::vision_signature_s_t> signatures) { this->signatures = signatures; }

float Vision::getDirection() {
    auto object = this->vision->get_by_size(0);
    if (object.signature == 255) return 0.0f;
    int centerX = object.left_coord + (object.width / 2) - (VISION_FOV_WIDTH / 2);
    float centerXPercentage = centerX / (VISION_FOV_WIDTH / 2.0f);
    float directionRad = std::atan(centerXPercentage * std::tan(lemlib::degToRad(61 / 2.0f)));
    return lemlib::radToDeg(directionRad);
}

}; // namespace lemlib