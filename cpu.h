#pragma once

#include <cstdint>

namespace avr {
    struct CPU {
        uint8_t R[32];

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

    };
}
