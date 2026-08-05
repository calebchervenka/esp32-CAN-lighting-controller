#include "vehicle_state.h"

#include "lighting_controller.h"
#include "animations/oneshot/Flash.h"
#include "animations/oneshot/Wipe.h"
#include "animations/oneshot/Fade.h"

FlashOneShot* activeFlashAnim = nullptr;

void updateSportMode(bool current_state) {
    if (current_state == vehicle_state.sport_mode_active) return;

    vehicle_state.sport_mode_active = current_state;

    if (current_state)
    {
        trigger_one_shot(new WipeRateOneShot(CRGB::OrangeRed, 28.0f));
    }
    else
    {
        trigger_one_shot(new FadeRateOneShot(CRGB::Red, 20.0f));
    }
}

void updateTractionCtl(bool current_state)
{
    if (current_state == vehicle_state.tcs_active) return;

    vehicle_state.tcs_active = current_state;
}

void updateRawRPMs(uint8_t raw_rpms)
{
    vehicle_state.raw_rpms = raw_rpms;

    bool rpms_high = raw_rpms > 0x60;
    if (rpms_high && !vehicle_state.rpms_high)
    {
        if (activeFlashAnim == nullptr) {
            activeFlashAnim = new FlashOneShot(CRGB::OrangeRed, 4000.0f, 10.0f);
        }
        trigger_one_shot(activeFlashAnim);
    }
    else if (!rpms_high && vehicle_state.rpms_high)
    {
        if (activeFlashAnim != nullptr) {
            activeFlashAnim->stop();
            activeFlashAnim = nullptr;
        }
    }
    vehicle_state.rpms_high = rpms_high;
}