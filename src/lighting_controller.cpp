#include "lighting_controller.h"

IndefiniteAnimation* baseAnimation = nullptr;
IndefiniteAnimation* fallbackBase = nullptr;

void set_base_animation(IndefiniteAnimation* anim) {
    if (anim != nullptr) {
        baseAnimation = anim;
    } else {
        baseAnimation = fallbackBase;
    }
}

IndefiniteAnimation* get_base_animation()
{
    return baseAnimation;
}

IndefiniteAnimation* get_fallback_base_animation()
{
    return fallbackBase;
}

void set_fallback_base_animation(IndefiniteAnimation* anim)
{
    if (anim != nullptr)
    {
        fallbackBase = anim;
    }
}

void trigger_one_shot(OneShotAnimation* anim) {
    if (!anim) return;

    OneShotNode* newNode = new OneShotNode(anim);
    if (!oneShotHead) {
        oneShotHead = newNode;
    } else {
        // Append to the end of the chain (renders on top of older one-shots)
        OneShotNode* curr = oneShotHead;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}