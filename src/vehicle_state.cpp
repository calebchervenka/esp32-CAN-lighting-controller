#include "vehicle_state.h"

#include "lighting_controller.h"
#include "animations/oneshot/Flash.h"
#include "animations/oneshot/Wipe.h"
#include "animations/oneshot/Fade.h"
#include "animations/oneshot/Pulse.h"
#include "config.h"

// High RPMs lighting event triggers when vehicle RPM signal crosses this threshold
#define RPMS_THRESHOLD 0x56

// CAN bus frame IDs
constexpr uint32_t FRAME_DRIVE_MODE(0x381);
constexpr uint32_t FRAME_RAW_RPMS(0x111);
constexpr uint32_t FRAME_TCS_ESC_CTL(0x153);
constexpr uint32_t FRAME_CRUISE_CTL(0x260);

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
    trigger_one_shot(new PulseOneShot(CRGB::SkyBlue, 12, !vehicle_state.tcs_active, 28.0f));
}

static void updateStabilityCtl(bool current_state)
{
    if (current_state == vehicle_state.esc_active) return;

    vehicle_state.esc_active = current_state;
    trigger_one_shot(new PulseOneShot(CRGB::Amethyst, 12, !vehicle_state.esc_active, 48.0f));
}

static void updateCruiseCtl(bool current_state)
{
    if (current_state == vehicle_state.cruise_active) return;

    vehicle_state.cruise_active = current_state;
    trigger_one_shot(new PulseOneShot(CRGB::Green, 12, !vehicle_state.cruise_active, 28.0f));
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

static void updatePRNDS(uint8_t data)
{
    if (data == 0x4e) return;

    TRANS_MODE trans_mode = TRANS_MODE(data & 0x0f);
    
    if (vehicle_state.trans_mode != trans_mode)
    {
        Serial.printf("New trans mode: %d\n", data & 0xff);
        switch ((static_cast<uint8_t>(vehicle_state.trans_mode) << 4) | static_cast<uint8_t>(trans_mode))
        {
            // Adjacent shifts
            case 0x07: Serial.println("P to R"); configBaseAnimation->setDirection(true); break; // P -> R
            case 0x70: Serial.println("R to P"); configBaseAnimation->setDirection(false); break; // R -> P
            case 0x76: Serial.println("R to N"); configBaseAnimation->setDirection(false); break; // R -> N
            case 0x67: Serial.println("N to R"); configBaseAnimation->setDirection(true); break; // N -> R
            case 0x65: Serial.println("N to D"); break; // N -> D
            case 0x56: Serial.println("D to N"); break; // D -> N
            case 0x58: Serial.println("D to S"); trigger_one_shot(new PulseOneShot(CRGB::Amethyst, 12, false, 24.0f)); break; // D -> S
            case 0x85: Serial.println("S to D"); trigger_one_shot(new PulseOneShot(CRGB::Amethyst, 12, true, 24.0f));break; // S -> D


            // Impossible shifts (without missed update)
            case 0x06: break; // P -> N
            case 0x05: break; // P -> D
            case 0x08: break; // P -> S
            case 0x75: break; // R -> D
            case 0x78: break; // R -> S
            case 0x60: break; // N -> P
            case 0x68: break; // N -> S
            case 0x50: break; // D -> P
            case 0x57: break; // D -> R
            case 0x80: break; // S -> P
            case 0x87: break; // S -> R
            case 0x86: break; // S -> N
            default: break;
        }
        vehicle_state.trans_mode = trans_mode;
        
    }
}

static void updateFrame_111(uint8_t* frame_data)
{
    updateRawRPMs(frame_data[6] & 0xff);
    updatePRNDS(frame_data[1]);
}

void updateVehicleState(twai_message_t frame)
{
    switch (frame.identifier)
    {
        case FRAME_DRIVE_MODE: updateSportMode(frame.data[0] & 0x04); break;
        case FRAME_RAW_RPMS: updateFrame_111(frame.data); break;
        case FRAME_TCS_ESC_CTL: updateTractionCtl(frame.data[0] & 0x04); updateStabilityCtl(frame.data[1] & 0x10); break;
        case FRAME_CRUISE_CTL: updateCruiseCtl(frame.data[3] & 0x02); break;
        default: return;
    }
}