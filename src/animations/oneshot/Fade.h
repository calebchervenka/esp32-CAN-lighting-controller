#pragma once

#include "animations/animations.h"
#include "animations/indefinite/Static.h"

#include "config.h"

class FadeOneShot : public OneShotAnimation
{
private:
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
        : OneShotAnimation(color, false), 
            startTime(0), durationMs(0), rate(rate) {};
    
    FadeOneShot(CRGB color, uint32_t durationMs)
        : OneShotAnimation(color, false), 
            startTime(0), durationMs(durationMs), rate(-1.0f) {};

    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override
    {
        if (ledCount == 0) return;
        if (startTime == 0) startTime = currentTime;

        uint32_t elapsed = currentTime - startTime;
        uint32_t durationMs = getDuration(ledCount);
        uint16_t blend_factor = elapsed * 255 / durationMs;
        if (blend_factor > 255) blend_factor = 255;
        fl::HSV16 blended_hsv = nblend(target[0], color, blend_factor);

        if (invDirection)
        {
            for (uint16_t i = 0; i < ledCount; i++)
            {
                target[ledCount - i - 1] = fl::HSV16(blended_hsv.h, blended_hsv.s, target[ledCount - i - 1].toHSV16().v);
            }
        }
        else
        {
            for (uint16_t i = 0; i < ledCount; i++)
            {
                target[i] = fl::HSV16(blended_hsv.h, blended_hsv.s, target[i].toHSV16().v);
            }
        }
    }

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        uint32_t durationMs = getDuration(ledCount);
        return (startTime != 0) && (currentTime - startTime >= durationMs);
    }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return rate > 0 ? max((uint32_t) 1, (uint32_t)(8000 / rate)) : durationMs;
    }

    void onFinish(IndefiniteAnimation& current_base)
    {
        current_base.color = color;
    }
};