#include "instructions/sbi.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& SBIInstruction::GetDestinationIO(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F8u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 3);
        return cpu.GPIO[index];
    }

    uint8_t SBIInstruction::GetSourceBit(uint16_t opcode) const
    {
        return static_cast<uint8_t>(opcode & 0x7u);
    }

    uint32_t SBIInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& io = GetDestinationIO(ctx.cpu, opcode);
        auto bit = GetSourceBit(opcode);
        auto mask = static_cast<uint8_t>(0x1u << bit);

        uint8_t value = io | mask;
        _clock.ConsumeCycle();

        io = value;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool SBIInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::SBI);
        auto mask = static_cast<uint16_t>(OpCodeMask::SBI);
        return (opcode & mask) == op;
    }
}
