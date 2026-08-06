#pragma once

#include "animations/animations.h"

class FlashOneShot : public OneShotAnimation
{
private:
    CRGB color;
    uint32_t startTime;
    uint32_t durationMs;

    uint8_t invRate;  // Number of milliseconds between each flash on/off
    mutable bool shouldStop;

public:
    FlashOneShot(CRGB color, uint16_t count, float rate) :
        color(color), startTime(0), durationMs(rate / count), invRate(1000/rate), shouldStop(false) {};
    FlashOneShot(CRGB color, float durationMs, float rate) :
        color(color), startTime(0), durationMs(durationMs), invRate(1000/rate), shouldStop(false) {};
    
    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override
    {
        if (ledCount == 0) return;
        if (startTime == 0) startTime = currentTime;
        uint32_t elapsed = currentTime - startTime;
        if ((elapsed % (2 * invRate)) > invRate)
        {
            fill_solid(target, ledCount, color);
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        if (shouldStop)
        {
            return true;
        }
        return ((startTime != 0) && ((currentTime - startTime) >= durationMs));
    }

    void stop()
    {
        shouldStop = true;
    }

    IndefiniteAnimation* getFinalState() const override { return nullptr; }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return durationMs;
    }
};