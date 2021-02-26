#pragma once

namespace avr {
    enum class OpCode : uint16_t {
        // Arithmetic 
        ADC    = 0x1c00,
        ADD    = 0x0c00,
        ADIW   = 0x9600,
        ASR    = 0x9C05,
        COM    = 0x9400,
        DEC    = 0x940A,
        EOR    = 0x2400,
        FMUL   = 0x0308,
        FMULS  = 0x0380,
        FMULSU = 0x0388,
        INC    = 0x9403,

        // Logic
        AND  = 0x2000,
        ANDI = 0x7000,

        // MISC
        BCLR  = 0x9488,
        BSET  = 0x9408,
        BLD   = 0xF800,
        BST   = 0xFA00,
        BREAK = 0x9598,
        CBI   = 0x9800,
        CP    = 0x1400,
        CPC   = 0x0400,
        CPI   = 0x3000,

        // Branching
        BRBC  = 0xF400,
        BRBS  = 0xF000,
        CALL  = 0x940E,
        ICALL = 0x9509,
        CPSE  = 0x1000,
        IJMP  = 0x9409,
        JMP   = 0x940C,

        // Read / Write
        LDS  = 0x9000,
        IN   = 0xB000,
        LAC  = 0x9206,
    };

    enum class OpCodeMask : uint16_t {
        // Arithmetic 
        ADC    = 0xFC00,
        ADD    = 0xFC00,
        ADIW   = 0xFF00,
        ASR    = 0xFE0F,
        COM    = 0xFE00,
        DEC    = 0xFE0F,
        EOR    = 0xFC00,
        FMUL   = 0xFF88,
        FMULS  = 0xFF88,
        FMULSU = 0xFF88,
        INC    = 0xFE0F,

        // Logic
        AND  = 0xFC00,
        ANDI = 0xF000,

        // MISC
        BCLR  = 0xFF8F,
        BSET  = 0xFF8F,
        BLD   = 0xFE08,
        BST   = 0xFE08,
        BREAK = 0xFFFF,
        CBI   = 0xFF00,
        CP    = 0xFC00,
        CPC   = 0xFC00,
        CPI   = 0xF000,

        // Branching
        BRBC  = 0xFC00,
        BRBS  = 0xFC00,
        CALL  = 0xFE0E,
        ICALL = 0xFFFF,
        CPSE  = 0xFC00,
        IJMP  = 0xFFFF,
        JMP   = 0xFE0E,

        // Read / Write
        LDS  = 0xFE0F,
        IN   = 0xF800,
        LAC  = 0xFE0F,
    };
}
