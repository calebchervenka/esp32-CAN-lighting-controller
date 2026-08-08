#include "CAN_comm.h"

#include <HardwareSerial.h>

#define SEND_GVRET_FRAMES_OVER_SERIAL 0

void initSerial()
{
    Serial.begin(115200);

    // ESP32-C6 USB CDC needs the host connection to be open before data is emitted.
    uint32_t start = millis();
    while (!Serial && (millis() - start) < 500) {
        delay(10);
    }

    delay(1000);
    Serial.write(0xE7);
    Serial.write(0xE7);
    Serial.flush();
}

void initTWAI() 
{
    can0.config.listen_only = true;
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_17, GPIO_NUM_16, can0.config.listen_only ? TWAI_MODE_LISTEN_ONLY : TWAI_MODE_NORMAL);
    g_config.rx_queue_len = 24;
    twai_timing_config_t t_config;
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (!getTWAITiming(can0.config.baud, t_config)) {
        t_config = TWAI_TIMING_CONFIG_500KBITS();
    }

    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err == ESP_OK) {
        err = twai_start();
        if (err == ESP_OK) {
            can0.twai.enabled = true;
            return;
        }
        twai_driver_uninstall();
    }

    can0.twai.enabled = false;
}

void deinitTWAI() 
{
    if (can0.twai.enabled) {
        twai_stop();
        twai_driver_uninstall();
    }
    can0.twai.enabled = false;
}

bool restartTWAI() {
    deinitTWAI();
    initTWAI();
    return can0.twai.enabled;
}

bool getTWAITiming(uint32_t baud, twai_timing_config_t &t_config) 
{
    switch (baud) {
        case 1000000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_1MBITS();
            return true;
        case 800000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_800KBITS();
            return true;
        case 500000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_500KBITS();
            return true;
        case 250000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_250KBITS();
            return true;
        case 125000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_125KBITS();
            return true;
        case 100000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_100KBITS();
            return true;
        case 50000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_50KBITS();
            return true;
        case 25000:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_25KBITS();
            return true;
        default:
            t_config = (twai_timing_config_t)TWAI_TIMING_CONFIG_500KBITS();
            return false;
    }
}

void handleCANFrames() 
{
    if (!can0.twai.enabled) return;

    twai_message_t frame;
    // Process ALL backlogged frames currently in queue
    while (twai_receive(&frame, 0) == ESP_OK) {
        updateVehicleState(frame);
        #if SEND_GVRET_FRAMES_OVER_SERIAL
        // sendGVRETFrameOverSerial(frame.identifier, frame.data_length_code, frame.data);
        #endif
    }
}

void handleSerialClient() 
{
    while (Serial.available() > 0) {
        parseByte((uint8_t)Serial.read());
    }
}

void processCommand(uint8_t cmd, uint8_t* buf, uint32_t len) {
    switch (cmd) {
        case 0x01: {
            uint32_t now_us = micros();
            uint8_t resp[6] = {
                0xF1, 0x01,
                (uint8_t)(now_us & 0xFF),
                (uint8_t)((now_us >> 8) & 0xFF),
                (uint8_t)((now_us >> 16) & 0xFF),
                (uint8_t)((now_us >> 24) & 0xFF)
            };
            Serial.write(resp, 6);
            break;
        }

        case 0x06: {
            uint8_t resp[12] = {0};
            resp[0] = 0xF1;
            resp[1] = 0x06;
            resp[2] = (can0.twai.enabled ? 1 : 0);
            resp[3] = (can0.config.baud | 0x80000000) & 0xFF;
            resp[4] = ((can0.config.baud | 0x80000000) >> 8) & 0xFF;
            resp[5] = ((can0.config.baud | 0x80000000) >> 16) & 0xFF;
            resp[6] = ((can0.config.baud | 0x80000000) >> 24) & 0xFF;
            Serial.write(resp, 12);
            break;
        }

        case 0x07: {
            uint8_t resp[8] = {0xF1, 0x07, 0xE9, 0x03, 0x00, 0x00, 0x00, 0x00};
            Serial.write(resp, 8);
            break;
        }

        case 0x09: {
            uint8_t resp[4] = {0xF1, 0x09, 0xDE, 0xAD};
            Serial.write(resp, 4);
            break;
        }

        case 0x0C: {
            uint8_t resp[3] = {0xF1, 0x0C, 0x01};
            Serial.write(resp, 3);
            break;
        }
    }
}

void parseByte(uint8_t c)
{
    if (!parser.is_binary_mode) {
        if (c == 0xE7) {
            parser.is_binary_mode = true;
            parser.state = STATE_IDLE;
        }
        return;
    }

    switch (parser.state) {
        case STATE_IDLE:
            if (c == 0xF1) {
                parser.state = STATE_CMD;
            }
            break;

        case STATE_CMD:
            parser.current_cmd = c;
            parser.rx.step = 0;
            if (parser.current_cmd == 0x00) {
                parser.state = STATE_READ_FRAME;
            } else if (parser.current_cmd == 0x05) {
                parser.state = STATE_READ_CONFIG;
            } else if (parser.current_cmd == 0x01 || parser.current_cmd == 0x06 || parser.current_cmd == 0x07 || parser.current_cmd == 0x09 || parser.current_cmd == 0x0C) {
                processCommand(parser.current_cmd, nullptr, 0);
                parser.state = STATE_IDLE;
            } else {
                parser.state = STATE_IDLE;
            }
            break;

        case STATE_READ_FRAME:
            parser.rx.buffer[parser.rx.step++] = c;
            if (parser.rx.step >= 6) {
                uint8_t data_len = parser.rx.buffer[5];
                if (data_len > 8) data_len = 8;
                if (parser.rx.step >= (uint32_t)(6 + data_len + 1)) {
                    processCommand(parser.current_cmd, parser.rx.buffer, parser.rx.step - 1);
                    parser.state = STATE_IDLE;
                }
            }
            break;

        case STATE_READ_CONFIG:
            parser.rx.buffer[parser.rx.step++] = c;
            if (parser.rx.step >= 9) {
                processCommand(parser.current_cmd, parser.rx.buffer, 8);
                parser.state = STATE_IDLE;
            }
            break;
    }
}

bool getBitFromFrame(const twai_message_t &frame, uint8_t byte_index, uint8_t bit_index) {
    if (bit_index >= (frame.data_length_code * 8)) {
        return false;
    }

    uint8_t bit_offset = bit_index % 8;
    return (frame.data[byte_index] >> bit_offset) & 0x01;
}

bool getBitFromFrame(const twai_message_t &frame, uint8_t bit_index) {
    uint8_t byte_index = bit_index / 8 - 1;
    uint8_t bit_offset = bit_index % 8;
    return getBitFromFrame(frame, byte_index, bit_offset);
}

void sendGVRETFrameOverSerial(uint32_t id, uint8_t dlc, const uint8_t* data) {
    #ifdef SEND_FRAMES_OVER_SERIAL
    uint8_t buf[20];
    buf[0] = 0xF1;
    buf[1] = 0x00;

    uint32_t timestamp = micros();
    buf[2] = timestamp & 0xFF;
    buf[3] = (timestamp >> 8) & 0xFF;
    buf[4] = (timestamp >> 16) & 0xFF;
    buf[5] = (timestamp >> 24) & 0xFF;

    buf[6] = id & 0xFF;
    buf[7] = (id >> 8) & 0xFF;
    buf[8] = (id >> 16) & 0xFF;
    buf[9] = (id >> 24) & 0xFF;

    buf[10] = dlc;
    buf[11] = 0;

    for (int i = 0; i < 8; i++) {
        buf[12 + i] = (i < dlc) ? data[i] : 0;
    }

    Serial.write(buf, sizeof(buf));
    // Serial.flush();
    #endif
}