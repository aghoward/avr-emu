#include "instructions/bclr.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t BCLRInstruction::GetSource(uint16_t opcode) const
    {
        auto mask = 0x0070;
        return static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
    }

    uint32_t BCLRInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto src = GetSource(opcode);
        auto flagValue = (opcode & 0x0080u) == 0u;

        if (src == 0u)
            ctx.cpu.SREG.C = flagValue;
        else if (src == 1u)
            ctx.cpu.SREG.Z = flagValue;
        else if (src == 2u)
            ctx.cpu.SREG.N = flagValue;
        else if (src == 3u)
            ctx.cpu.SREG.V = flagValue;
        else if (src == 4u)
            ctx.cpu.SREG.S = flagValue;
        else if (src == 5u)
            ctx.cpu.SREG.H = flagValue;
        else if (src == 6u)
            ctx.cpu.SREG.T = flagValue;
        else if (src == 7u)
            ctx.cpu.SREG.I = flagValue;

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
