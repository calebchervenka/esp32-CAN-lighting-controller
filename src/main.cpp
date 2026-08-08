#include <Arduino.h>

#include "lighting_controller.h"
#include "CAN_comm.h"
#include "vehicle_state.h"
#include "config.h"

// Application
void setup() {
    initSerial();
    lighting_controller_setup(configBaseAnimation);
    initTWAI();
}

void loop() {
    handleCANFrames();
    handleSerialClient();
    lighting_controller_update();
    delay(1);
}