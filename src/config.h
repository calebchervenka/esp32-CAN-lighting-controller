#pragma once

#include <FastLED.h>
#include "animations/animations.h"
#include "animations/indefinite/Waves.h"
#include "animations/indefinite/Static.h"

inline IndefiniteAnimation* configBaseAnimation = new StaticColorAnimation(CRGB::Red);