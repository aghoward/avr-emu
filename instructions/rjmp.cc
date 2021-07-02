#include "instructions/rjmp.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    int16_t RJMPInstruction::GetAddressOffset(uint16_t opcode) const
    {
        auto address = static_cast<int16_t>(opcode & 0x7FFu);
        if ((opcode & 0x800u) != 0u)
            address |= static_cast<int16_t>(0xF800);
        _clock.ConsumeCycle();
        return address;
    }

    uint32_t RJMPInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        ctx.cpu.PC = static_cast<uint16_t>(ctx.cpu.PC + GetAddressOffset(opcode));
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool RJMPInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::RJMP);
        auto mask = static_cast<uint16_t>(OpCodeMask::RJMP);
        return (opcode & mask) == op;
    }
}
