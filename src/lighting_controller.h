#pragma once

#include <memory.h>

#define FASTLED_INTERNAL
#define FASTLED_ESP32_RAW_PIN_ORDER
#include <FastLED.h>
#include <fl/hsv16.h>

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
    OneShotNode* prev;
    OneShotNode* next;

    OneShotNode(OneShotAnimation* a, OneShotNode* parent) : anim(a), prev(parent), next(nullptr) {}
    OneShotNode(OneShotAnimation* a) : anim(a), prev(nullptr), next(nullptr) {}
    ~OneShotNode() { }
};

inline OneShotNode* oneShotHead = nullptr;

/**
 * @brief Set or change the background/indefinite animation loop.
 */
void set_base_animation(IndefiniteAnimation* anim);

IndefiniteAnimation* get_base_animation();

/**
 * @brief Set or change the fallback background/indefinite animation. This animation is used when an invalid animation is provided to a `set_base_animation()` call.
 */
void set_fallback_base_animation(IndefiniteAnimation* anim);

IndefiniteAnimation* get_fallback_base_animation();

/**
 * @brief Initialize FastLED hardware.
 */
inline void lighting_controller_setup(IndefiniteAnimation* fallback) {
    FastLED.addLeds<WS2812B, LED_PIN_LEFT, GRB>(leds_left, LED_COUNT_LEFT);
    FastLED.addLeds<WS2812B, LED_PIN_RIGHT, GRB>(leds_right, LED_COUNT_RIGHT);
    FastLED.clear(true);
    set_fallback_base_animation(fallback);
    set_base_animation(get_fallback_base_animation());
}

/**
 * @brief Add a one-shot animation to the one-shot animation stack.
 * @return Address to the created node.
 */
OneShotNode* trigger_one_shot(OneShotAnimation* anim);

/**
 * @brief Deletes the node's animation, removes the node from the one-shot animation stack, and deletes the node object.
 * Note: The pointer passed to this method will be invalid.
 */
void cancel_one_shot(OneShotNode** pNode);

/**
 * @brief Main engine loop. Call every loop execution.
 * Handles rendering, layering, and garbage collection.
 */
inline void lighting_controller_update() {
    uint32_t now = millis();

    // 1. Render Base Layer
    get_base_animation()->render(leds_left, now, true, LED_COUNT_LEFT);
    get_base_animation()->render(leds_right, now, false, LED_COUNT_RIGHT);

    // 2. Render One-Shot Layers & Auto-Cull
    OneShotNode** currPtr = &oneShotHead;

    while (*currPtr != nullptr) {
        OneShotNode* node = *currPtr;

        const bool leftFinished = node->anim->isFinished(now, LED_COUNT_LEFT);
        const bool rightFinished = node->anim->isFinished(now, LED_COUNT_RIGHT);

        if (leftFinished && rightFinished) {
            node->anim->onFinish(*get_base_animation());
            
            node->anim->render(leds_left, now, true, LED_COUNT_LEFT);
            node->anim->render(leds_right, now, false, LED_COUNT_RIGHT);

            cancel_one_shot(&node);
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