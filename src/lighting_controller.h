#pragma once

#include <memory.h>

#define FASTLED_INTERNAL
#define FASTLED_ESP32_RAW_PIN_ORDER
#include <FastLED.h>

#include "animations/animations.h"

// System configuration
#ifndef LED_COUNT_LEFT
#define LED_COUNT_LEFT 144
#endif

#ifndef LED_COUNT_RIGHT
#define LED_COUNT_RIGHT 288
#endif

#ifndef LED_PIN_LEFT
#define LED_PIN_LEFT 15
#endif

#ifndef LED_PIN_RIGHT
#define LED_PIN_RIGHT 14
#endif

inline CRGB leds_left[LED_COUNT_LEFT];           // Internal engine state
inline CRGB layerBuffer_left[LED_COUNT_LEFT];    // Buffer for layer compositing

inline CRGB leds_right[LED_COUNT_RIGHT];           // Internal engine state
inline CRGB layerBuffer_right[LED_COUNT_RIGHT];    // Buffer for layer compositing

// Linked list node for dynamic One-Shots
struct OneShotNode {
    OneShotAnimation* anim;
    OneShotNode* next;

    OneShotNode(OneShotAnimation* a) : anim(a), next(nullptr) {}
    ~OneShotNode() { delete anim; }
};

inline IndefiniteAnimation* baseAnimation = nullptr;
inline IndefiniteAnimation* fallbackBase = nullptr;
inline OneShotNode* oneShotHead = nullptr;

/**
 * @brief Initialize FastLED hardware. Call once in setup().
 */
inline void lighting_controller_setup(IndefiniteAnimation* fallback) {
    FastLED.addLeds<WS2812B, LED_PIN_LEFT, GRB>(leds_left, LED_COUNT_LEFT);
    FastLED.addLeds<WS2812B, LED_PIN_RIGHT, GRB>(leds_right, LED_COUNT_RIGHT);
    FastLED.clear(true);
    fallbackBase = fallback;
    baseAnimation = fallbackBase;
}

/**
 * @brief Set or change the background/indefinite animation loop.
 */
void set_base_animation(IndefiniteAnimation* anim);

/**
 * @brief Add a one-shot animation to the stack.
 * Transfers ownership of the pointer to the controller (smart management).
 */
void trigger_one_shot(OneShotAnimation* anim);

/**
 * @brief Main engine loop. Call every loop execution.
 * Handles rendering, layering, and garbage collection.
 */
inline void lighting_controller_update() {
    uint32_t now = millis();

    // 1. Render Base Layer
    baseAnimation->render(leds_left, now, true, LED_COUNT_LEFT);
    baseAnimation->render(leds_right, now, false, LED_COUNT_RIGHT);

    // 2. Render One-Shot Layers & Auto-Cull
    OneShotNode** currPtr = &oneShotHead;

    while (*currPtr != nullptr) {
        OneShotNode* node = *currPtr;

        const bool leftFinished = node->anim->isFinished(now, LED_COUNT_LEFT);
        const bool rightFinished = node->anim->isFinished(now, LED_COUNT_RIGHT);

        if (leftFinished && rightFinished) {
            IndefiniteAnimation* final_state = node->anim->getFinalState();
            if (final_state)
            {
                fallbackBase = final_state;
                baseAnimation = fallbackBase;
                baseAnimation->render(leds_left, now, true, LED_COUNT_LEFT);
                baseAnimation->render(leds_right, now, false, LED_COUNT_RIGHT);
            }

            // Unlink & free memory
            *currPtr = node->next;
            delete node; // Automatically invokes OneShotNode destructor -> deletes node->anim
        } else {
            // Render active layer on top
            node->anim->render(leds_left, now, true, LED_COUNT_LEFT);
            node->anim->render(leds_right, now, false, LED_COUNT_RIGHT);
            currPtr = &(node->next);
        }
    }

    // 3. Output to strip
    FastLED.show();
}