#include "instructions/ijmp.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint32_t IJMPInstruction::Execute(uint16_t, CPU& cpu, SRAM&) const
    {
        cpu.PC = *cpu.Z;
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
