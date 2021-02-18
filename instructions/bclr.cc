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
        auto flagValue = (opcode & 0x0080u) == 0u;

        if (src == 0u)
            cpu.SREG.C = flagValue;
        else if (src == 1u)
            cpu.SREG.Z = flagValue;
        else if (src == 2u)
            cpu.SREG.N = flagValue;
        else if (src == 3u)
            cpu.SREG.V = flagValue;
        else if (src == 4u)
            cpu.SREG.S = flagValue;
        else if (src == 5u)
            cpu.SREG.H = flagValue;
        else if (src == 6u)
            cpu.SREG.T = flagValue;
        else if (src == 7u)
            cpu.SREG.I = flagValue;

        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool BCLRInstruction::Matches(OpCode op, OpCodeMask mask, uint16_t opcode) const
    {
        return (opcode & static_cast<uint16_t>(mask)) == static_cast<uint16_t>(op);
    }

    bool BCLRInstruction::Matches(uint16_t opcode) const
    {
        return Matches(OpCode::BCLR, OpCodeMask::BCLR, opcode) ||
            Matches(OpCode::BSET, OpCodeMask::BSET, opcode);
    }
}
