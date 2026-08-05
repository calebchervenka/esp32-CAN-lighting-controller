#pragma once

#include "animations/animations.h"
#include "animations/indefinite/Static.h"

class WipeDurationOneShot : public OneShotAnimation {
private:
    CRGB color;
    uint32_t startTime;
    uint32_t durationMs;

public:
    /** 
     * @brief Wipes from one side to the other, updating the base color afterward.
     * @param color Color to wipe in.
     * @param durationMs Duration for which the animation should run; rate adjusts accordingly.
     */
    WipeDurationOneShot(CRGB color, uint32_t durationMs = 1000)
        : color(color), durationMs(durationMs), startTime(0) {}

    void render(CRGB* target, uint32_t currentTime, uint16_t ledCount) override {
        if (startTime == 0) startTime = currentTime;
        uint32_t elapsed = currentTime - startTime;
        
        uint16_t litCount = map(min(elapsed, durationMs), 0, durationMs, 0, ledCount);
        for (uint16_t i = 0; i < litCount; ++i) {
            target[i] = color; // Alpha blend over lower layers
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override {
        return (startTime != 0) && (currentTime - startTime >= durationMs);
    }

    IndefiniteAnimation* getFinalState() const override { return new StaticColorAnimation(color); }
};

class WipeRateOneShot : public OneShotAnimation
{
private:
    CRGB color;
    uint32_t startTime;
    float rate;

public:
    /** 
     * @brief Wipes from one side to the other, updating the base color afterward.
     * @param color Color to wipe in.
     * @param rate Rate at which the animation should run; duration adjusts accordingly.
     * 
     * A rate of `10.0f` is equivalent to a duration of `1000ms` with `144` LEDs.
     */
    WipeRateOneShot(CRGB color, float rate = 10.0f)
        : color(color), startTime(0), rate(rate) {};
    
    void render(CRGB* target, uint32_t currentTime, uint16_t ledCount) override
    {
        if (startTime == 0) startTime = currentTime;
        uint32_t elapsed = currentTime - startTime;

        uint32_t durationMs = getDuration(ledCount);

        uint16_t litCount = map(min(elapsed, durationMs), 0, durationMs, 0, ledCount);
        for (uint16_t i = 0; i < litCount; ++i) {
            target[i] = color; // Alpha blend over lower layers
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
        return 1/(rate*0.0144)*ledCount;
    }
};