#include "instructions/spm.h"
#include "core/cpu.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint32_t SPMInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto source = IndirectRegister(&ctx.cpu.R[0]);
        auto destination = IndirectRegister(&ctx.progMem[*ctx.cpu.Z]);
        destination = *source;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }
    

    bool SPMInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::SPM);
        auto mask = static_cast<uint16_t>(OpCodeMask::SPM);
        return (opcode & mask) == op;
    }
}
