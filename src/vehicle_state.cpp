#include "vehicle_state.h"

#include "lighting_controller.h"
#include "animations/oneshot/Flash.h"
#include "animations/oneshot/Wipe.h"
#include "animations/oneshot/Fade.h"
#include "animations/oneshot/Pulse.h"

#define RPMS_THRESHOLD 0x56

FlashOneShot* activeFlashAnim = nullptr;

static void updateSportMode(bool current_state) {
    if (current_state == vehicle_state.sport_mode_active) return;

    vehicle_state.sport_mode_active = current_state;

    if (current_state)
    {
        trigger_one_shot(new WipeOneShot(CRGB::OrangeRed, 28.0f));
    }
    else
    {
        trigger_one_shot(new FadeOneShot(CRGB::Red, 20.0f));
    }
}

static void updateTractionCtl(bool current_state)
{
    if (current_state == vehicle_state.tcs_active) return;

    vehicle_state.tcs_active = current_state;
    trigger_one_shot(new PulseRateOneShot(CRGB::SkyBlue, 12, !vehicle_state.tcs_active, 28.0f));
}

static void updateStabilityCtl(bool current_state)
{
    if (current_state == vehicle_state.esc_active) return;

    vehicle_state.esc_active = current_state;
    trigger_one_shot(new PulseRateOneShot(CRGB::Amethyst, 12, !vehicle_state.esc_active, 48.0f));
}

static void updateCruiseCtl(bool current_state)
{
    if (current_state == vehicle_state.cruise_active) return;

    vehicle_state.cruise_active = current_state;
    trigger_one_shot(new PulseRateOneShot(CRGB::Green, 12, !vehicle_state.cruise_active, 28.0f));
}

static void updateRawRPMs(uint8_t raw_rpms)
{
    vehicle_state.raw_rpms = raw_rpms;

    bool rpms_high = raw_rpms > RPMS_THRESHOLD;
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

void updateVehicleState(twai_message_t frame)
{
    if (frame.identifier == 0x381)
    {
        updateSportMode(frame.data[0] & 0x04);
    }
    else if (frame.identifier == 0x111)
    {
        updateRawRPMs(frame.data[6] & 0xff);
    }
    else if (frame.identifier == 0x153)
    {
        updateTractionCtl(frame.data[0] & 0x04);
        updateStabilityCtl(frame.data[1] & 0x10);
    }
    else if (frame.identifier == 0x260)
    {
        updateCruiseCtl(frame.data[3] & 0x02);
    }
}