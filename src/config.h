#pragma once

#include <FastLED.h>
#include "animations/animations.h"
#include "animations/indefinite/Waves.h"
#include "animations/indefinite/Static.h"

inline IndefiniteAnimation* configBaseAnimation = new StaticColorAnimation(CRGB::Red);

#define COLOR_SPORT_MODE CRGB::OrangeRed
#define COLOR_NORMAL_MODE CRGB::Red
#define COLOR_TRACTION_CONTROL_TOGGLED CRGB::OrangeRed
#define COLOR_STABILITY_CONTROL_TOGGLED CRGB::Amethyst