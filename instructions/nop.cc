#include "instructions/nop.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint32_t NOPInstruction::Execute(uint16_t, ExecutionContext&) const
    {
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool NOPInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::NOP);
        auto mask = static_cast<uint16_t>(OpCodeMask::NOP);
        return (opcode & mask) == op;
    }
}
