#pragma once

#include "animations/animations.h"

class PulseAnimation : public IndefiniteAnimation {
private:
    CRGB color;
    uint32_t periodMs;

public:
    PulseAnimation(CRGB color, uint32_t periodMs = 2000)
        : color(color), periodMs(periodMs) {}

    void render(CRGB* target, uint32_t currentTime, uint16_t ledCount) override {
        // Smooth sine wave brightness modulation [0-255]
        uint8_t bright = quadwave8((currentTime * 255) / periodMs);
        fill_solid(target, ledCount, color);
        nscale8_video(target, ledCount, bright);
    }
};