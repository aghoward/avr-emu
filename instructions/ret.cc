#include "instructions/ret.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint16_t RETInstruction::GetAddress(ExecutionContext& ctx) const
    {
        uint16_t address = 0u;
        for (auto i = 0u; i < sizeof(ctx.cpu.PC); i++)
        {
            _clock.ConsumeCycle();
            auto shift = 8 * (sizeof(ctx.cpu.PC) - i - 1);
            address |= static_cast<uint16_t>(
                (ctx.ram[++ctx.cpu.SP] << shift) & (0xffu << shift));
        }
        return address;
    }

    uint32_t RETInstruction::Execute(uint16_t, ExecutionContext& ctx) const
    {
        ctx.cpu.PC = GetAddress(ctx);
        _clock.ConsumeCycle();
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool RETInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::RET);
        auto mask = static_cast<uint16_t>(OpCodeMask::RET);
        return (opcode & mask) == op;
    }
}
