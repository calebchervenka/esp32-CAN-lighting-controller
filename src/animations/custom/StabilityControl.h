#pragma once

#include "animations/oneshot/Pulse.h"

#include "config.h"

class StabilityControlToggleOneShot : public PulseOneShot
{
public:
    StabilityControlToggleOneShot(bool esc_active)
        : PulseOneShot(COLOR_STABILITY_CONTROL_TOGGLED, 12, !esc_active, 48.0f) {};
};