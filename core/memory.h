#pragma once

#include <memory>
#ifndef AVR_EMU_RAM_SIZE
#define AVR_EMU_RAM_SIZE 0x800u    // 2 K
#endif

#ifndef AVR_EMU_FLASH_SIZE
#define AVR_EMU_FLASH_SIZE 0x4000u // 16 K
#endif

#include <cstdint>

namespace avr {
    class Memory {
        private:
            std::unique_ptr<uint8_t[]> _data;
            std::size_t _size;

        public:
            Memory(std::size_t size)
                : _data(std::make_unique<uint8_t[]>(size)),
                  _size(size)
            {}

            Memory() = delete;

            uint8_t& operator[](uint16_t address)
            {
                return _data[address % _size];
            }

            const uint8_t& operator[](uint16_t address) const
            {
                return _data[address % _size];
            }

            constexpr std::size_t size() const
            {
                return _size;
            }
    };

    using SRAM = Memory;
    using ProgramMemory = Memory;
}
