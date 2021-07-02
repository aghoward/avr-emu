#include "instructions/pop.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& POPInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        auto value = static_cast<uint8_t>((opcode & mask) >> 4);
        return cpu.R[value];
    }

    uint8_t POPInstruction::GetStackValue(ExecutionContext& ctx) const
    {
        _clock.ConsumeCycle();
        return ctx.ram[++ctx.cpu.SP];
    }

    uint32_t POPInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto value = GetStackValue(ctx);

        rd = value;

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool POPInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::POP);
        auto mask = static_cast<uint16_t>(OpCodeMask::POP);
        return (opcode & mask) == op;
    }
}
