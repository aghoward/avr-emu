#include "instructions/push.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& PUSHInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        auto value = static_cast<uint8_t>((opcode & mask) >> 4);
        return cpu.R[value];
    }

    void PUSHInstruction::SetStackValue(ExecutionContext& ctx, uint8_t value) const
    {
        _clock.ConsumeCycle();
        ctx.ram[ctx.cpu.SP--] = value;
    }

    uint32_t PUSHInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetSourceRegister(ctx.cpu, opcode);
        SetStackValue(ctx, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool PUSHInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::PUSH);
        auto mask = static_cast<uint16_t>(OpCodeMask::PUSH);
        return (opcode & mask) == op;
    }
}
