#pragma once

#include "animations/oneshot/Pulse.h"

#include "config.h"

class TractionControlToggleOneShot : public PulseOneShot
{
public:
    TractionControlToggleOneShot(bool tcs_active)
        : PulseOneShot(COLOR_TRACTION_CONTROL_TOGGLED, 12, !tcs_active, 28.0f) {};
};