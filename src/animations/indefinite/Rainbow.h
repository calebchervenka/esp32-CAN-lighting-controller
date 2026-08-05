#pragma once

#include "animations/animations.h"

class RainbowAnimation : public IndefiniteAnimation {
private:
    uint8_t speed;
    uint8_t deltaHue;

public:
    RainbowAnimation(uint8_t speed = 20, uint8_t deltaHue = 7)
        : speed(speed), deltaHue(deltaHue) {}

    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override 
    {
        uint8_t startHue = (currentTime * speed) / 1000;
        fill_rainbow(target, ledCount, startHue, deltaHue);
    }
};