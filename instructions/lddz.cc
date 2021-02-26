#include "instructions/lddz.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LDDZInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint8_t LDDZInstruction::GetDisplacement(uint16_t opcode) const
    {
        return static_cast<uint8_t>(((opcode >> 7u) & 0x18u) |
            ((opcode >> 8u) & 0x20) |
            (opcode & 0x7u));
    }

    uint32_t LDDZInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        if (IsPreDecrement(opcode))
            --ctx.cpu.Z;

        _clock.ConsumeCycle();
        auto displacement = IsDisplaced(opcode) ? GetDisplacement(opcode) : static_cast<uint16_t>(0u);
        rd = ctx.ram[static_cast<uint16_t>(*ctx.cpu.Z + displacement)];
        _clock.ConsumeCycle();

        if (IsPostIncrement(opcode))
            ctx.cpu.Z++;

        return _cyclesConsumed;
    }

    bool LDDZInstruction::IsPostIncrement(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x9001u);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return (opcode & mask) == op;
    }

    bool LDDZInstruction::IsPreDecrement(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x9002u);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return (opcode & mask) == op;
    }

    bool LDDZInstruction::IsDisplaced(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x8000u);
        auto mask = static_cast<uint16_t>(0xD208u);
        return (opcode & mask) == op;
    }

    bool LDDZInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LDDZ);
        auto mask = static_cast<uint16_t>(OpCodeMask::LDDZ);
        return (opcode & mask) == op ||
            IsPostIncrement(opcode) ||
            IsPreDecrement(opcode) ||
            IsDisplaced(opcode);
    }
}
