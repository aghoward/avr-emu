#include "instructions/lsr.h"
#include "instructions/opcodes.h"

#include <cstdint>
#include <iostream>

namespace avr {
    uint8_t& LSRInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    void LSRInstruction::SetRegisterFlags(CPU& cpu, uint8_t rd, uint8_t result) const
    {
        cpu.SREG.N = false;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = (rd & 0x1u) != 0u;
        cpu.SREG.V = cpu.SREG.C;
        cpu.SREG.S = cpu.SREG.C;
    }

    uint32_t LSRInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        auto result = static_cast<uint8_t>(rd >> 1u);
        SetRegisterFlags(ctx.cpu, rd, result);
        rd = result;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LSRInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LSR);
        auto mask = static_cast<uint16_t>(OpCodeMask::LSR);
        return (opcode & mask) == op;
    }
}
