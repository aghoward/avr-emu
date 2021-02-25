#include "instructions/break.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint32_t BREAKInstruction::Execute(uint16_t, CPU&, SRAM&) const
    {
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool BREAKInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::BREAK);
        auto mask = static_cast<uint16_t>(OpCodeMask::BREAK);
        return (opcode & mask) == op;
    }
}
