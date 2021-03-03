#include "instructions/movw.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t* MOVWInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x0F;
        uint8_t value = static_cast<uint8_t>((opcode & mask) << 1u);
        return std::addressof(cpu.R[value]);
    }

    uint8_t* MOVWInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0xF0;
        uint8_t value = static_cast<uint8_t>((opcode & mask) >> 3u);
        return std::addressof(cpu.R[value]);
    }

    uint32_t MOVWInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto* rr = GetSourceRegister(ctx.cpu, opcode);
        auto* rd = GetDestinationRegister(ctx.cpu, opcode);

        for (auto i = 0u; i < sizeof(uint16_t); i++)
            *(rd + i) = (*(rr + i) >> (i * 8));
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool MOVWInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::MOVW);
        auto mask = static_cast<uint16_t>(OpCodeMask::MOVW);
        return (opcode & mask) == op;
    }
}
