#pragma once

#include "animations/oneshot/Pulse.h"

#include "config.h"

class CruiseControlToggleOneShot : public PulseOneShot
{
public:
    CruiseControlToggleOneShot(bool cc_active)
        : PulseOneShot(CRGB::Green, 12, !cc_active, 28.0f) {};
};