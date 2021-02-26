#include "instructions/lpm.h"
#include "instructions/opcodes.h"

#include <cstdint>
#include <iostream>

namespace avr {
    uint8_t& LPMInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        if (!HasRegister(opcode))
            return cpu.R[0];
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LPMInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        _clock.ConsumeCycle();
        rd = ctx.progMem[*ctx.cpu.Z];
        _clock.ConsumeCycle();

        if (IsPostIncrement(opcode))
            ctx.cpu.Z++;

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool LPMInstruction::IsPostIncrement(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x9005u);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return (opcode & mask) == op;
    }

    bool LPMInstruction::HasRegister(uint16_t opcode) const
    {
        auto op   = static_cast<uint16_t>(0x9004u);
        auto mask = static_cast<uint16_t>(0xFE0Fu);
        return ((opcode & mask) == op) || IsPostIncrement(opcode);
    }

    bool LPMInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LPM);
        auto mask = static_cast<uint16_t>(OpCodeMask::LPM);
        return (opcode & mask) == op ||
            HasRegister(opcode);
    }
}
