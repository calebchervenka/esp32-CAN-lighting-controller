#pragma once

#include "driver/twai.h"
#include "vehicle_state.h"

enum PARSER_STATE
{
    STATE_IDLE,
    STATE_CMD,
    STATE_READ_FRAME,
    STATE_READ_CONFIG
};

struct
{
    bool is_binary_mode = false;
    
    PARSER_STATE state = STATE_IDLE;

    uint8_t current_cmd = 0;

    struct {
        uint8_t buffer[100];
        uint32_t step = 0;
    } rx;
} parser;

struct
{
    struct {
        bool enabled = false;
    } twai;

    struct {
        uint32_t baud = 500000;
        bool listen_only = false;
    } config;
} can0;

// Two-wire automotive interface methods
void initSerial();
void initTWAI();
void deinitTWAI();
bool restartTWAI();
bool getTWAITiming(uint32_t baud, twai_timing_config_t &t_config);

// CAN-bus methods
void handleCANFrames();
void handleFrameUpdate(const twai_message_t &frame);
void handleSerialClient();

// Data methods
void processCommand(uint8_t cmd, uint8_t* buf, uint32_t len);
void parseByte(uint8_t c);
bool getBitFromFrame(const twai_message_t &frame, uint8_t byte_index, uint8_t bit_index);
bool getBitFromFrame(const twai_message_t &frame, uint8_t bit_index);
void sendGVRETFrameOverSerial(uint32_t id, uint8_t dlc, const uint8_t* data);