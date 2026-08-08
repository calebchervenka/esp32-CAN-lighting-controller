#pragma once
#include <cstdint>

#include "driver/twai.h"

enum class TRANS_MODE : uint8_t
{
    P = 0x00, 
    R = 0x07, 
    N = 0x06, 
    D = 0x05, 
    S = 0x08
};

// Vehicle state
typedef struct __vehicle_state_t
{
    bool sport_mode_active = false;
    bool tcs_active = false;
    bool esc_active = false;
    bool cruise_active = false;
    uint8_t raw_rpms = 0;
    bool rpms_high = false;
    TRANS_MODE trans_mode = TRANS_MODE::P;
} vehicle_state_t;

inline vehicle_state_t vehicle_state;

void updateVehicleState(twai_message_t frame);