#pragma once

#include "core/memory.h"

#include <cstdint>
#include <memory>

namespace avr {
    struct CPU {
        constexpr static uint16_t R_END = 0x1Fu;
        constexpr static uint16_t GPIO_END = 0x5Fu;
        constexpr static uint16_t SRAM_BEG = 0x60u;

        uint8_t * R;  // 32 Memory Mapped General Purpose Registers
        uint8_t * GPIO; // 64 Memory Mapped GPIO Registers

        uint16_t PC;
        uint16_t SP;

        struct {
            uint8_t C : 1,
                    Z : 1,
                    N : 1,
                    V : 1,
                    S : 1,
                    H : 1,
                    T : 1,
                    I : 1;
        } SREG;

        uint8_t RAMPX;
        uint8_t RAMPY;
        uint8_t RAMPZ;
        uint8_t RAMPD;
        uint8_t EIND;

        CPU(Memory& mem) :
            R(std::addressof(mem[0x00])),
            GPIO(std::addressof(mem[R_END + 1u])),
            PC(0x0u),
            SP(0x0u),
            RAMPX(0x0u),
            RAMPY(0x0u),
            RAMPD(0x0u),
            EIND(0x0u)
        {
            SREG.C = SREG.Z = SREG.N = SREG.V = SREG.S = SREG.H = SREG.T = SREG.I = false;
        }
    };
}
