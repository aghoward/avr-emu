#pragma once

namespace avr {
    enum class OpCode : uint16_t {
        // Arithmetic 
        ADC  = 0x1c00,
        ADD  = 0x0c00,
        ADIW = 0x9600,
        ASR  = 0x9C05,

        // Logic
        AND  = 0x2000,
        ANDI = 0x7000,

        // MISC
        BCLR = 0x9488,
        BSET = 0x9408,
        BLD  = 0xF800,
        BREAK = 0x9598,

        // Branching
        BRBC = 0xF400,
        BRBS = 0xF000,
    };

    enum class OpCodeMask : uint16_t {
        // Arithmetic 
        ADC  = 0xFC00,
        ADD  = 0xFC00,
        ADIW = 0xFF00,
        ASR  = 0xFE0F,

        // Logic
        AND  = 0xFC00,
        ANDI = 0xF000,

        // MISC
        BCLR = 0xFF8F,
        BSET = 0xFF8F,
        BLD  = 0xFE08,
        BREAK = 0xFFFF,

        // Branching
        BRBC = 0xFC00,
        BRBS = 0xFC00,
    };
}
