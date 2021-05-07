#include "instructions/neg.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& NEGInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    void NEGInstruction::SetStatusRegisters(CPU& cpu, uint8_t rd, uint8_t result) const
    {
        cpu.SREG.H = (result & 0x04u) || (rd & 0x04u);
        cpu.SREG.V = result == 0x80u;
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = !cpu.SREG.Z;
    }

    uint32_t NEGInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        auto result = static_cast<uint8_t>((rd ^ 0xFFu) + 1u);
        SetStatusRegisters(ctx.cpu, rd, result);
        rd = result;

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool NEGInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::NEG);
        auto mask = static_cast<uint16_t>(OpCodeMask::NEG);
        return (opcode & mask) == op;
    }
}
