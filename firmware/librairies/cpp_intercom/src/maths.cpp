#include "maths.h"
#include <Arduino.h>

uint32_t CRC_POLY = 0x864cfb;
uint32_t CRC_INIT = 0xb704ce;
uint32_t MSB_MASK = (uint32_t)1 << 23;
uint32_t CRC_MASK = ((MSB_MASK - 1) << 1) | 1;

uint32_t crc24(String inputs) {
    uint32_t crc = CRC_INIT;
    for (unsigned int i = 0; i < inputs.length(); i++) {
        crc ^= (uint32_t)inputs[i] << 16;
        for (int j = 0; j < 8; j++) {
            crc <<= 1;
            if (crc & MSB_MASK) {
                crc ^= CRC_POLY;
            }
        }
    }

    return crc & CRC_MASK;
}