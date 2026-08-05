#pragma once

#include "animations/animations.h"
#include "animations/indefinite/Static.h"

class FadeOneShot : public OneShotAnimation
{
private:
    CRGB color;
    uint32_t startTime;
    const uint32_t durationMs;
    float rate;

public:
    /**
     * @brief Fades the current output toward a new color over time and then locks the base to that color.
     * @param color Target color to fade to.
     * @param rate Rate at which the animation should run; duration adjusts accordingly.
     */
    FadeOneShot(CRGB color, float rate = 10.0f)
        : color(color), startTime(0), durationMs(0), rate(rate) {}
    
    FadeOneShot(CRGB color, uint32_t durationMs)
        : color(color), startTime(0), durationMs(durationMs), rate(-1.0f) {}

    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override
    {
        if (startTime == 0) startTime = currentTime;

        uint32_t elapsed = currentTime - startTime;
        uint32_t durationMs = getDuration(ledCount);
        uint8_t blendAmount = (durationMs == 0) ? 255 : (uint8_t)map(min(elapsed, durationMs), 0, durationMs, 0, 255);

        for (uint16_t i = 0; i < ledCount; ++i) {
            nblend(target[i], color, blendAmount);
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        uint32_t durationMs = getDuration(ledCount);
        return (startTime != 0) && (currentTime - startTime >= durationMs);
    }

    IndefiniteAnimation* getFinalState() const override { return new StaticColorAnimation(color); }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return rate > 0 ? max((uint32_t) 1, (uint32_t)(8000 / rate)) : durationMs;
    }
};