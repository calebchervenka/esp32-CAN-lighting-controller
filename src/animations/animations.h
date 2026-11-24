#pragma once

#include <FastLED.h>

class Animation {
protected:

    Animation(CRGB color, bool invDirection) :
        color(color), invDirection(invDirection) {};
public:
    CRGB color;
    bool invDirection;

    virtual ~Animation() = default;

    /**
     * @brief Renders the animation state into a canvas buffer.
     * @param target CRGB buffer.
     * @param currentTime Current timestamp in milliseconds.
     * @param ledCount Size of CRGB buffer.
     */
    virtual void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) = 0;

    void setColor(CRGB newColor)
    {
        color = newColor;
    }

    void setDirection(bool inverted)
    {
        invDirection = inverted;
    }

    bool toggleDirection()
    {
        invDirection = !invDirection;
        return invDirection;
    }
};

class IndefiniteAnimation : public Animation
{
protected:
    IndefiniteAnimation(CRGB color, bool invDirection)
        : Animation(color, invDirection) {};
};

class OneShotAnimation : public Animation 
{
protected:
    OneShotAnimation(CRGB color, bool invDirection)
        : Animation(color, invDirection) {};

public:
    virtual ~OneShotAnimation() = default;

    /**
     * @return true if the one-shot sequence is complete.
     */
    virtual bool isFinished(uint32_t currentTime, uint16_t ledCount) const = 0;

    /**
     * @brief Is executed when a one-shot animation finishes. Use to modify the existing base animation.
     */
    virtual void onFinish(IndefiniteAnimation& current_base) = 0;
};