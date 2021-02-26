#include "instructions/icall.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    void ICALLInstruction::PushReturnAddress(CPU& cpu, SRAM& mem) const
    {
        uint16_t returnAddress = cpu.PC;
        for (uint16_t i = 0u; i < sizeof(cpu.PC); i++) {
            _clock.ConsumeCycle();
            mem[cpu.SP--] = static_cast<uint8_t>((returnAddress >> (i * 8u)) & 0xFFu);
        }
    }

    uint32_t ICALLInstruction::Execute(uint16_t, ExecutionContext& ctx) const
    {
        PushReturnAddress(ctx.cpu, ctx.ram);
        ctx.cpu.PC = *ctx.cpu.Z;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool ICALLInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::ICALL);
        auto mask = static_cast<uint16_t>(OpCodeMask::ICALL);
        return (opcode & mask) == op;
    }
}
