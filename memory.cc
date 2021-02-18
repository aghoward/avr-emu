#include "memory.h"

#include <cstdint>

namespace avr {
    uint8_t& Memory::operator[](uint16_t address) {
        return _data[address];
    }

    const uint8_t& Memory::operator[](uint16_t address) const {
        return _data[address];
    }
}
