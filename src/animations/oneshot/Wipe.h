#pragma once

#include "animations/animations.h"
#include "animations/indefinite/Static.h"

#include "config.h"
#include "fl/hsv16.h"

class WipeOneShot : public OneShotAnimation
{
private:
    uint32_t startTime;
    const uint32_t durationMs;
    const float rate;

public:
    WipeOneShot(CRGB color, uint32_t durationMs)
        : OneShotAnimation(color, false),
            startTime(0), durationMs(durationMs), rate(-1.0f) {};
    
    WipeOneShot(CRGB color, float rate)
        : OneShotAnimation(color, false),
            startTime(0), durationMs(0), rate(rate) {};
    
    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override
    {
        if (ledCount == 0) return;
        if (startTime == 0) startTime = currentTime;

        uint32_t elapsed = currentTime - startTime;
        uint32_t durationMs = getDuration(ledCount);
        uint16_t litCount = map(min(elapsed, durationMs), 0, durationMs, 0, ledCount);

        if (invDirection)
        {
            for (uint16_t i = 0; i < litCount; i++)
            {
                target[ledCount - i - 1] = fl::HSV16(color.toHSV16().h, color.toHSV16().s, target[ledCount - i - 1].toHSV16().v);
            }
        }
        else
        {
            for (uint16_t i = 0; i < litCount; ++i) 
            {
                // target[i] = color;
                target[i] = fl::HSV16(color.toHSV16().h, color.toHSV16().s, target[i].toHSV16().v);
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
        current_base.color = color;
    }

    uint32_t getDuration(uint16_t ledCount) const
    {
        return rate > 0 ? 1/(rate*0.0144)*ledCount : durationMs;
    }
};