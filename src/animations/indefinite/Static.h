#pragma once

#include "animations/animations.h"

class StaticColorAnimation : public IndefiniteAnimation {
private:
    CRGB color;

public:
    StaticColorAnimation(CRGB color) : color(color) {}

    void setColor(CRGB newColor) { color = newColor; }

    void render(CRGB* target, uint32_t currentTime, bool invDirection, uint16_t ledCount) override 
    {
        fill_solid(target, ledCount, color);
    }
};