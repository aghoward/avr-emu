#include "instructions/ror.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& RORInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void RORInstruction::SetRegisterFlags(CPU& cpu, uint8_t original, uint8_t result) const
    {
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = (original & 0x01u) != 0u;
        cpu.SREG.V = cpu.SREG.N ^ cpu.SREG.C;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }

    uint32_t RORInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto originalValue = rd;
        rd = (rd >> 1) | (ctx.cpu.SREG.C << 7);
        SetRegisterFlags(ctx.cpu, originalValue, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool RORInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::ROR);
        auto mask = static_cast<uint16_t>(OpCodeMask::ROR);
        return (opcode & mask) == op;
    }
}
