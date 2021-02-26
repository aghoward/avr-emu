#include "instructions/ldd.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LDDInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint8_t LDDInstruction::GetDisplacement(uint16_t opcode) const
    {
        return static_cast<uint8_t>(((opcode >> 7u) & 0x18u) |
            ((opcode >> 8u) & 0x20) |
            (opcode & 0x7u));
    }

    uint32_t LDDInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        if (IsPreDecrement(opcode))
            --ctx.cpu.Y;

        _clock.ConsumeCycle();
        auto displacement = IsDisplaced(opcode) ? GetDisplacement(opcode) : static_cast<uint16_t>(0u);
        rd = ctx.ram[static_cast<uint16_t>(*ctx.cpu.Y + displacement)];
        _clock.ConsumeCycle();

        if (IsPostIncrement(opcode))
            ctx.cpu.Y++;

        return _cyclesConsumed;
    }

    bool LDDInstruction::IsPostIncrement(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x9009u);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return (opcode & mask) == op;
    }

    bool LDDInstruction::IsPreDecrement(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x900Au);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return (opcode & mask) == op;
    }

    bool LDDInstruction::IsDisplaced(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x8008u);
        auto mask = static_cast<uint16_t>(0xD208u);
        return (opcode & mask) == op;
    }

    bool LDDInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LDD);
        auto mask = static_cast<uint16_t>(OpCodeMask::LDD);
        return (opcode & mask) == op ||
            IsPostIncrement(opcode) ||
            IsPreDecrement(opcode) ||
            IsDisplaced(opcode);
    }
}
