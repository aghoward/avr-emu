#include "instructions/stx.h"
#include "core/cpu.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    bool STXInstruction::IsPreDecrement(uint16_t opcode) const
    {
        return (opcode & 0x02u) != 0u;
    }

    bool STXInstruction::IsPostIncrement(uint16_t opcode) const
    {
        return (opcode & 0x01u) != 0u;
    }

    uint8_t STXInstruction::GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const
    {
        auto mask = 0x1F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4u);
        return ctx.cpu.R[index];
    }

    uint32_t STXInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto source = GetSourceRegister(opcode, ctx);
        _clock.ConsumeCycle();
        auto& index = ctx.cpu.X;
        if (IsPreDecrement(opcode))
            index--;
        auto destination = IndirectRegister(&ctx.ram[*index]);
        if (IsPostIncrement(opcode))
            index++;
        destination = source;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }
    

    bool STXInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::STX);
        auto mask = static_cast<uint16_t>(OpCodeMask::STX);
        return (opcode & mask) == op;
    }
}
