#pragma once

#include "animations/oneshot/Wipe.h"
#include "animations/oneshot/Fade.h"
#include "config.h"

class SportModeEnableOneShot : public WipeOneShot
{
public:
    /** 
     * @brief Wipes from one side to the other. Custom animation defined for when sport mode is enabled.
     * 
     */
    SportModeEnableOneShot()
        : WipeOneShot(COLOR_SPORT_MODE, 28.0f) {};

    void onFinish(IndefiniteAnimation& current_base)
    {
        current_base.color = color;
    }
};

class SportModeDisableOneShot : public FadeOneShot
{
public:
    /**
     * @brief Fades the current output toward a new color over time and then locks the base to that color.
     * @param color Target color to fade to.
     * @param rate Rate at which the animation should run; duration adjusts accordingly.
     */
    SportModeDisableOneShot()
        : FadeOneShot(COLOR_NORMAL_MODE, 20.0f) {};

    void onFinish(IndefiniteAnimation& current_base)
    {
        current_base.color = color;
    }
};