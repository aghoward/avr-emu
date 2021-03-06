#pragma once

#ifndef AVR_EMU_RAM_SIZE
#define AVR_EMU_RAM_SIZE 0x900u
#endif

#ifndef AVR_EMU_FLASH_SIZE
#define AVR_EMU_FLASH_SIZE 0x4000u
#endif

#include <cstdint>

namespace avr {
    template <std::size_t Size>
    class Memory {
        private:
            uint8_t _data[Size];
        public:
            uint8_t& operator[](uint16_t address)
            {
                return _data[address];
            }

            const uint8_t& operator[](uint16_t address) const
            {
                return _data[address];
            }
    };

    using SRAM = Memory<AVR_EMU_RAM_SIZE>;
    using ProgramMemory = Memory<AVR_EMU_FLASH_SIZE>;
}
