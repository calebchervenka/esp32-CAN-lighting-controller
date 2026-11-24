#pragma once

#include "Flash.h"

class InfiniteFlashOneShot : public FlashOneShot
{
    
public:
    InfiniteFlashOneShot(CRGB color, uint16_t count, float rate) 
        : FlashOneShot(color, count, rate) {};

    bool isFinished(uint32_t currentTime, uint16_t ledCount) const override
    {
        return false;
    }
};