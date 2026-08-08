#pragma once

#include "animations/animations.h"

class PulseOneShot : public OneShotAnimation
{
private:
    uint32_t startTime;
    uint8_t pulseWidth;
    bool invDirection_;
    float rate;

public:
    /** 
     * @brief Fires a pulse from one side to the other.
     * @param color Color to pulse.
     * @param rate Rate at which the animation should run; duration adjusts accordingly.
     * 
     * A rate of `10.0f` is equivalent to a duration of `1000ms` with `144` LEDs.
     */
    PulseOneShot(CRGB color, uint8_t pulseWidth, bool invDirection, float rate = 10.0f)
        : OneShotAnimation(color, invDirection),
            startTime(0), pulseWidth(pulseWidth), invDirection_(invDirection), rate(rate) {};
    
    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override
    {
        if (ledCount == 0) return;
        if (startTime == 0) startTime = currentTime;
        uint32_t elapsed = currentTime - startTime;

        uint32_t durationMs = getDuration(ledCount);

        uint16_t litCount = map(min(elapsed, durationMs), 0, durationMs, 0, (ledCount + pulseWidth));
        uint16_t unlitCount = max(litCount - pulseWidth, 0);

        if (invDirection ^ invDirection_)
        {
            for (uint16_t i = unlitCount; i < litCount && i < ledCount; ++i) {
                target[ledCount - i - 1] = color; // Alpha blend over lower layers
            }
        }
        else
        {
            for (uint16_t i = unlitCount; i < litCount && i < ledCount; ++i) {
                target[i] = color; // Alpha blend over lower layers
            }
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        uint32_t durationMs = getDuration(ledCount);
        return (startTime != 0) && (currentTime - startTime >= durationMs);
    }

    void onFinish(IndefiniteAnimation& current_base)
    {
        // current_base_color = color;
        return;
    }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return 1/(rate*0.0144)*(ledCount+pulseWidth);
    }
};