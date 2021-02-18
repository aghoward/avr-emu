#include "instructions/bclr.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t BCLRInstruction::GetSource(uint16_t opcode) const
    {
        auto mask = 0x0070;
        return static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
    }

    uint32_t BCLRInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto src = GetSource(opcode);

        if (src == 0u)
            cpu.SREG.C = 0;
        else if (src == 1u)
            cpu.SREG.Z = 0;
        else if (src == 2u)
            cpu.SREG.N = 0;
        else if (src == 3u)
            cpu.SREG.V = 0;
        else if (src == 4u)
            cpu.SREG.S = 0;
        else if (src == 5u)
            cpu.SREG.H = 0;
        else if (src == 6u)
            cpu.SREG.T = 0;
        else if (src == 7u)
            cpu.SREG.I = 0;

        return _cyclesConsumed;
    }

    bool BCLRInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::BCLR);
        auto mask = static_cast<uint16_t>(OpCodeMask::BCLR);
        return (opcode & mask) == op;
    }
}
