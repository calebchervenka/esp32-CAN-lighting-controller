#pragma once

#include <FastLED.h>

class Animation {
public:
    virtual ~Animation() = default;

    /**
     * @brief Renders the animation state into a canvas buffer.
     * @param target CRGB buffer.
     * @param currentTime Current timestamp in milliseconds.
     * @param ledCount Size of CRGB buffer.
     */
    virtual void render(CRGB* target, uint32_t currentTime, uint16_t ledCount) = 0;
};

class IndefiniteAnimation : public Animation {};

class OneShotAnimation : public Animation {
    IndefiniteAnimation* final_state = nullptr;
public:
    virtual ~OneShotAnimation() = default;

    /**
     * @return true if the one-shot sequence is complete.
     */
    virtual bool isFinished(uint32_t currentTime, uint16_t ledCount) const = 0;

    /**
     * @return The final color this one-shot leaves behind after ending.
     */
    virtual IndefiniteAnimation* getFinalState() const = 0;
};