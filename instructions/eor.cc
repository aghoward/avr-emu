#include "instructions/eor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& EORInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x020F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.R[value];
    }

    uint8_t& EORInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void EORInstruction::SetRegisterFlags(CPU& cpu, uint8_t result) const
    {
        cpu.SREG.V = false;
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }

    uint32_t EORInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        rd ^= rr;
        SetRegisterFlags(ctx.cpu, rd);

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool EORInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::EOR);
        auto mask = static_cast<uint16_t>(OpCodeMask::EOR);
        return (opcode & mask) == op;
    }
}
