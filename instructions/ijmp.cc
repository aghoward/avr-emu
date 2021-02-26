#include "instructions/ijmp.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint32_t IJMPInstruction::Execute(uint16_t, ExecutionContext& ctx) const
    {
        ctx.cpu.PC = *ctx.cpu.Z;
        _clock.ConsumeCycle();
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool IJMPInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::IJMP);
        auto mask = static_cast<uint16_t>(OpCodeMask::IJMP);
        return (opcode & mask) == op;
    }
}
