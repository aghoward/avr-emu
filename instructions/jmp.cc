#include "instructions/jmp.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint16_t JMPInstruction::GetDestinationAddress(CPU& cpu, ProgramMemory& mem) const
    {
        auto address = static_cast<uint16_t>(0u);
        for (auto i = 0u; i < sizeof(address); i++)
        {
            address |= static_cast<uint16_t>(mem[cpu.PC++] << (i * 8u));
            _clock.ConsumeCycle();
        }
        // Actual avr assembly has this shifted right one bit to prevent odd addresses
        return (address << 1u);
    }

    uint32_t JMPInstruction::Execute(uint16_t, ExecutionContext& ctx) const
    {
        ctx.cpu.PC = GetDestinationAddress(ctx.cpu, ctx.progMem);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool JMPInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::JMP);
        auto mask = static_cast<uint16_t>(OpCodeMask::JMP);
        return (opcode & mask) == op;
    }
}
