#pragma once

#ifndef AVR_EMU_RAM_SIZE
#define AVR_EMU_RAM_SIZE 64u*1024u - 1u
#endif

#include <cstdint>

namespace avr {
    class Memory {
        private:
            uint8_t _data[AVR_EMU_RAM_SIZE];
        public:
            uint8_t& operator[](uint16_t address);
            const uint8_t& operator[](uint16_t address) const;
    };
}
