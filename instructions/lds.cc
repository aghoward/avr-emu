#include "instructions/lds.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint16_t LDSInstruction::GetImmediate(ExecutionContext& ctx) const
    {
        auto out = static_cast<uint16_t>(0u);
        for (auto i = 0u; i < sizeof(out); i++)
            out |= static_cast<uint16_t>(ctx.progMem[ctx.cpu.PC++] << (i * 8u));
        return out;
    }

    uint8_t& LDSInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LDSInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto k = GetImmediate(ctx);

        _clock.ConsumeCycle();
        rd = ctx.ram[k];
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LDSInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LDS);
        auto mask = static_cast<uint16_t>(OpCodeMask::LDS);
        return (opcode & mask) == op;
    }
}
