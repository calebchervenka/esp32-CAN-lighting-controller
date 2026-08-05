#include "lighting_controller.h"

void set_base_animation(IndefiniteAnimation* anim) {
    if (anim != nullptr) {
        baseAnimation = anim;
    } else {
        baseAnimation = fallbackBase;
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