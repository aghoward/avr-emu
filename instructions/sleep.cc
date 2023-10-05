#include "instructions/sleep.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint32_t SLEEPInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        ctx.cpu.is_sleeping = true;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool SLEEPInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::SLEEP);
        auto mask = static_cast<uint16_t>(OpCodeMask::SLEEP);
        return (opcode & mask) == op;
    }
}
