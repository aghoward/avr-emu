#include "instructions/cbi.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& CBIInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F8u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 3);
        return cpu.GPIO[index];
    }

    uint8_t CBIInstruction::GetSourceBit(uint16_t opcode) const
    {
        return static_cast<uint8_t>(opcode & 0x7u);
    }

    uint32_t CBIInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto src = GetSourceBit(opcode);
        auto mask = static_cast<uint8_t>(0xFFu ^ (0x1u << src));

        uint8_t value = rd & mask;
        _clock.ConsumeCycle();

        rd = value;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool CBIInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::CBI);
        auto mask = static_cast<uint16_t>(OpCodeMask::CBI);
        return (opcode & mask) == op;
    }
}
