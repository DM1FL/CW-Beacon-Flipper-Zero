#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef struct {
    const char* text;
    uint16_t wpm;
} MorseConfig;
void morse_encode_and_send(const MorseConfig* cfg, void (*tx_cb)(bool on, uint32_t duration_ms));