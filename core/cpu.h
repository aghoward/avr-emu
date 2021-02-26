#pragma once

#include "core/memory.h"

#include <cstdint>
#include <memory>

namespace avr {
    struct IndirectRegister {
    private:
        uint8_t* const _base;

    public:
        IndirectRegister(uint8_t* base)
            : _base(base)
        {}

        uint16_t operator*() const
        {
            uint16_t out = 0u;
            for (auto i = 0u; i < sizeof(out); i++)
                out |= static_cast<uint16_t>(_base[i] << (i * 8u));
            return out;
        }

        uint16_t operator=(uint16_t value)
        {
            for (auto i = 0u; i < sizeof(value); i++)
                _base[i] = (value >> (i * 8u)) & 0xFFu;
            return value;
        }

        uint16_t operator--()
        {
            auto value = static_cast<uint16_t>(this->operator*() - 1u);
            *this = value;
            return value;
        }

        uint16_t operator++()
        {
            auto value = static_cast<uint16_t>(this->operator*() + 1u);
            *this = value;
            return value;
        }

        uint16_t operator--(int)
        {
            auto value = this->operator*();
            *this = static_cast<uint16_t>(value - 1u);
            return value;
        }

        uint16_t operator++(int)
        {
            auto value = this->operator*();
            *this = static_cast<uint16_t>(value + 1u);
            return value;
        }
    };

    struct CPU {
        constexpr static uint16_t R_END = 0x1Fu;
        constexpr static uint16_t GPIO_END = 0x5Fu;
        constexpr static uint16_t EGPIO_END = 0xFFu;
        constexpr static uint16_t SRAM_BEG = 0x100u;

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

        IndirectRegister X;
        IndirectRegister Y;
        IndirectRegister Z;
        uint8_t RAMPX;
        uint8_t RAMPY;
        uint8_t RAMPZ;
        uint8_t RAMPD;
        uint8_t EIND;

        CPU(SRAM& mem) :
            R(std::addressof(mem[0x00])),
            GPIO(std::addressof(mem[R_END + 1u])),
            PC(0x0u),
            SP(0x0u),
            X(std::addressof(R[26])),
            Y(std::addressof(R[28])),
            Z(std::addressof(R[30])),
            RAMPX(0x0u),
            RAMPY(0x0u),
            RAMPD(0x0u),
            EIND(0x0u)
        {
            SREG.C = SREG.Z = SREG.N = SREG.V = SREG.S = SREG.H = SREG.T = SREG.I = false;
        }
    };
}
