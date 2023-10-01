#pragma once

#include <cstdint>

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
        LSR    = 0x9406,
        MUL    = 0x9C00,
        MULS   = 0x0200,
        MULSU  = 0x0300,
        ROL    = 0x1C00,
        ROR    = 0x9407,
        SBC    = 0x0800,

        // Logic
        AND  = 0x2000,
        ANDI = 0x7000,
        NEG  = 0x9401,
        OR   = 0x2800,
        ORI  = 0x6000,

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
        NOP   = 0x0000,

        // Branching
        BRBC  = 0xF400,
        BRBS  = 0xF000,
        CALL  = 0x940E,
        ICALL = 0x9509,
        CPSE  = 0x1000,
        IJMP  = 0x9409,
        JMP   = 0x940C,
        RCALL = 0xD000,
        RET   = 0x9508,
        RETI  = 0x9518,
        RJMP  = 0xC000,

        // Read / Write
        LDS  = 0x9000,
        IN   = 0xB000,
        LAC  = 0x9206,
        LAS  = 0x9205,
        LAT  = 0x9207,
        LD   = 0x900C,
        LDD  = 0x8008,
        LDDZ = 0x8000,
        LDI  = 0xE000,
        LPM  = 0x95C8,
        MOV  = 0x2C00,
        MOVW = 0x0100,
        OUT  = 0xB800,
        POP  = 0x900F,
        PUSH = 0x920F,
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
        LSR    = 0xFE0F,
        MUL    = 0xFC00,
        MULS   = 0xFF00,
        MULSU  = 0xFF88,
        ROL    = 0xFC00,
        ROR    = 0xFE0F,
        SBC    = 0xFC00,

        // Logic
        AND  = 0xFC00,
        ANDI = 0xF000,
        NEG  = 0xFE0F,
        OR   = 0xFC00,
        ORI  = 0xF000,

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
        NOP   = 0xFFFF,

        // Branching
        BRBC  = 0xFC00,
        BRBS  = 0xFC00,
        CALL  = 0xFE0E,
        ICALL = 0xFFFF,
        CPSE  = 0xFC00,
        IJMP  = 0xFFFF,
        JMP   = 0xFE0E,
        RCALL = 0xF000,
        RET   = 0xFFFF,
        RETI  = 0xFFFF,
        RJMP  = 0xF000,

        // Read / Write
        LDS  = 0xFE0F,
        IN   = 0xF800,
        LAC  = 0xFE0F,
        LAS  = 0xFE0F,
        LAT  = 0xFE0F,
        LD   = 0xFE0C,
        LDD  = 0xFE0F,
        LDDZ = 0xFE0F,
        LDI  = 0xF000,
        LPM  = 0xFFFF,
        MOV  = 0xFC00,
        MOVW = 0xFF00,
        OUT  = 0xF800,
        POP  = 0xFE0F,
        PUSH = 0xFE0F,
    };
}
