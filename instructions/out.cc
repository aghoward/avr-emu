#include "instructions/out.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& OUTInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x060F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.GPIO[value];
    }

    uint8_t& OUTInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    uint32_t OUTInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        rd = rr;

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool OUTInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::OUT);
        auto mask = static_cast<uint16_t>(OpCodeMask::OUT);
        return (opcode & mask) == op;
    }
}
