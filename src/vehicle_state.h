#pragma once
#include <cstdint>

// Vehicle state
typedef struct __vehicle_state_t
{
    bool sport_mode_active = false;
    bool tcs_active = false;
    uint8_t raw_rpms = 0;
    bool rpms_high = false;
} vehicle_state_t;

namespace
{
    vehicle_state_t vehicle_state;
}

// Lighting methods
void updateSportMode(bool current_state);
void updateTractionCtl(bool current_state);
void updateRawRPMs(uint8_t raw_rpms);