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

OneShotNode* trigger_one_shot(OneShotAnimation* anim) {
    // If the provided animation pointer is not valid, return nullptr for the node because we're not going to create one
    if (!anim) return nullptr;

    // If we have no head, create it and return
    if (!oneShotHead)
    {
        oneShotHead = new OneShotNode(anim);
        return oneShotHead;
    }

    // Because we DO have a head, follow the linked list down until we reach the end
    OneShotNode* pFinalNode = oneShotHead;
    while (pFinalNode->next)
    {
        pFinalNode = pFinalNode->next;
    }

    // Point the final node's next member to our new animation node
    OneShotNode* pNewNode = new OneShotNode(anim, pFinalNode);
    pFinalNode->next = pNewNode;
    return pNewNode;
}

/**
 * @brief Removes the one-shot animation from the animation stack and deletes the provided reference.
 */
void cancel_one_shot(OneShotNode** ppNode)
{
    OneShotNode* pNode = *ppNode;

    if (!pNode) return;

    // Update previous node
    if (pNode->prev)
    {
        // Has previous node
        pNode->prev->next = pNode->next;
    }
    else if (oneShotHead == pNode)
    {
        oneShotHead = pNode->next;
    }

    // Update next node
    if (pNode->next)
    {
        pNode->next->prev = pNode->prev;
    }


    // At this point, all references in the linked list should be deleted and we can safely delete the pNode and its animation


    // Cancel the animation
    if (pNode->anim)
    {
        delete pNode->anim;
        pNode->anim = nullptr;
    }

    // Delete the node and its references that we were provided
    delete pNode;
    *ppNode = nullptr;

    return;
}