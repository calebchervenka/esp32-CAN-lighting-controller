#pragma once

#include "animations/animations.h"

class WavesAnimation : public IndefiniteAnimation {
private:
    
    uint8_t speed;
    uint32_t startTime;
    uint8_t high;
    uint8_t low;
    uint32_t periodMs;

public:
    WavesAnimation(CRGB color, uint8_t speed = 20, uint8_t high = 255, uint8_t low = 0, bool invDirection = false, uint32_t periodMs = 1000)
        : IndefiniteAnimation(color, invDirection), speed(speed), startTime(0), high(high), low(low), periodMs(periodMs) {};

    void render(CRGB* target, uint32_t currentTime, bool invDirection_, uint16_t ledCount) override 
    {
        if (startTime == 0) startTime = currentTime;

        // fill_solid(target, ledCount, color);

        bool direction = (invDirection ^ invDirection_);
        for (int i = 0; i < ledCount; i++)
        {
            uint8_t waveVal = quadwave8((currentTime + i * speed) * 318 / periodMs);

            if (direction)
            {
                target[i] = color.scale8(waveVal);
            }
            else
            {
                target[ledCount - i - 1] = color.scale8(waveVal);
            }

        }
    }
};