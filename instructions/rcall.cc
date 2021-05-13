#include "instructions/rcall.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    void RCALLInstruction::PushReturnAddress(CPU& cpu, SRAM& mem) const
    {
        uint16_t returnAddress = cpu.PC;
        for (uint16_t i = 0u; i < sizeof(cpu.PC); i++) {
            _clock.ConsumeCycle();
            mem[cpu.SP--] = static_cast<uint8_t>((returnAddress >> (i * 8u)) & 0xFFu);
        }
    }

    int16_t RCALLInstruction::GetAddress(uint16_t opcode) const
    {
        auto value = static_cast<int16_t>(opcode & 0x0FFF);
        if ((value & 0x0800) != 0)
            value = static_cast<int16_t>(value | 0xF000);
        return value;
    }

    uint32_t RCALLInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        PushReturnAddress(ctx.cpu, ctx.ram);
        ctx.cpu.PC = static_cast<uint16_t>(ctx.cpu.PC + GetAddress(opcode));
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool RCALLInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::RCALL);
        auto mask = static_cast<uint16_t>(OpCodeMask::RCALL);
        return (opcode & mask) == op;
    }
}
