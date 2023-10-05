#include "instructions/stz.h"
#include "core/cpu.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    bool STZInstruction::IsPreDecrement(uint16_t opcode) const
    {
        return (opcode & 0x1002u) == 0x1002u;
    }

    bool STZInstruction::IsPostIncrement(uint16_t opcode) const
    {
        return (opcode & 0x1001u) == 0x1001u;
    }

    uint8_t STZInstruction::GetDisplacement(uint16_t opcode) const
    {
        if ((opcode & 0x1000u) != 0u)
            return 0u;
        return ((opcode & 0x2000u) >> 8u) | ((opcode & 0x0C00u) >> 7u) | (opcode & 0x0007u);
    }

    uint8_t STZInstruction::GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const
    {
        auto mask = static_cast<uint16_t>(0x1F0u);
        auto index = static_cast<uint16_t>((opcode & mask) >> 4u);
        return ctx.cpu.R[index];
    }

    uint32_t STZInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto source = GetSourceRegister(opcode, ctx);
        auto displacement = GetDisplacement(opcode);
        _clock.ConsumeCycle();
        auto& index = ctx.cpu.Z;
        if (IsPreDecrement(opcode))
            index--;
        auto destination = IndirectRegister(&ctx.ram[(*index) + displacement]);
        if (IsPostIncrement(opcode))
            index++;
        destination = source;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool STZInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::STZ);
        auto mask = static_cast<uint16_t>(OpCodeMask::STZ);
        return (opcode & mask) == op;
    }
}
