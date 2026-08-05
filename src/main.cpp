#include <Arduino.h>

#include "lighting_controller.h"
#include "animations/indefinite/Static.h"
#include "CAN_comm.h"
#include "vehicle_state.h"

// Animations
StaticColorAnimation redStatic(CRGB::Red);

// Application
void setup() {
    initSerial();
    lighting_controller_setup(&redStatic);
    initTWAI();
}

void loop() {
    handleCANFrames();
    handleSerialClient();
    lighting_controller_update();
    delay(1);
}