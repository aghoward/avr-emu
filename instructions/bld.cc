#include "instructions/bld.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t BLDInstruction::GetSourceValue(uint16_t opcode) const
    {
        auto mask = 0x0007u;
        return static_cast<uint8_t>(opcode & mask);
    }

    uint8_t& BLDInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    uint32_t BLDInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto bitShift = GetSourceValue(opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        if (ctx.cpu.SREG.T)
            rd |= static_cast<uint8_t>(ctx.cpu.SREG.T << bitShift);
        else
            rd &= 0xFFu ^ static_cast<uint8_t>(0x1u << bitShift);

        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool BLDInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::BLD);
        auto mask = static_cast<uint16_t>(OpCodeMask::BLD);
        return (opcode & mask) == op;
    }
}
