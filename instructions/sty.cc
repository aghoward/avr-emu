#include "instructions/sty.h"
#include "core/cpu.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    bool STYInstruction::IsPreDecrement(uint16_t opcode) const
    {
        return (opcode & 0x100Au) == 0x100Au;
    }

    bool STYInstruction::IsPostIncrement(uint16_t opcode) const
    {
        return (opcode & 0x1009u) == 0x1009u;
    }

    uint8_t STYInstruction::GetDisplacement(uint16_t opcode) const
    {
        if ((opcode & 0x1000u) != 0u)
            return 0u;
        return ((opcode & 0x2000u) >> 8u) | ((opcode & 0x0C00u) >> 7u) | (opcode & 0x0007u);
    }

    uint8_t STYInstruction::GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const
    {
        auto mask = static_cast<uint16_t>(0x1F0u);
        auto index = static_cast<uint16_t>((opcode & mask) >> 4u);
        return ctx.cpu.R[index];
    }

    uint32_t STYInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto source = GetSourceRegister(opcode, ctx);
        auto displacement = GetDisplacement(opcode);
        _clock.ConsumeCycle();
        auto& index = ctx.cpu.Y;
        if (IsPreDecrement(opcode))
            index--;
        ctx.ram[(*index) + displacement] = source;
        if (IsPostIncrement(opcode))
            index++;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool STYInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::STY);
        auto mask = static_cast<uint16_t>(OpCodeMask::STY);
        return (opcode & mask) == op;
    }
}
