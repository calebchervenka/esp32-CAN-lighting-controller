#pragma once

#include "animations/animations.h"

class PulseRateOneShot : public OneShotAnimation
{
private:
    CRGB color;
    uint32_t startTime;
    uint8_t pulseWidth;
    float rate;

public:
    /** 
     * @brief Fires a pulse from one side to the other.
     * @param color Color to pulse.
     * @param rate Rate at which the animation should run; duration adjusts accordingly.
     * 
     * A rate of `10.0f` is equivalent to a duration of `1000ms` with `144` LEDs.
     */
    PulseRateOneShot(CRGB color, uint8_t pulseWidth, float rate = 10.0f)
        : color(color), startTime(0), pulseWidth(pulseWidth), rate(rate) {};
    
    void render(CRGB* target, uint32_t currentTime, uint16_t ledCount) override
    {
        if (startTime == 0) startTime = currentTime;
        uint32_t elapsed = currentTime - startTime;

        uint32_t durationMs = getDuration(ledCount);

        uint16_t litCount = map(min(elapsed, durationMs), 0, durationMs, 0, (ledCount + pulseWidth));
        uint16_t unlitCount = max(litCount - pulseWidth, 0);
        for (uint16_t i = unlitCount; i < litCount && i < ledCount; ++i) {
            target[i] = color; // Alpha blend over lower layers
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        uint32_t durationMs = getDuration(ledCount);
        return (startTime != 0) && (currentTime - startTime >= durationMs);
    }

    IndefiniteAnimation* getFinalState() const override { return nullptr; }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return 1/(rate*0.0144)*(ledCount+pulseWidth);
    }
};