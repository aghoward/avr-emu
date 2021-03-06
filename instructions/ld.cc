#include "instructions/ld.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LDInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LDInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        if (IsPreDecrement(opcode))
            --ctx.cpu.X;

        _clock.ConsumeCycle();
        rd = ctx.ram[*ctx.cpu.X];
        _clock.ConsumeCycle();

        if (IsPostIncrement(opcode))
            ctx.cpu.X++;

        return _cyclesConsumed;
    }

    bool LDInstruction::IsPostIncrement(uint16_t opcode) const
    {
        return (opcode & 0x1u) != 0u;
    }

    bool LDInstruction::IsPreDecrement(uint16_t opcode) const
    {
        return (opcode & 0x2u) != 0u;
    }

    bool LDInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LD);
        auto mask = static_cast<uint16_t>(OpCodeMask::LD);
        return (opcode & mask) == op;
    }
}
