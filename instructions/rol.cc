#include "instructions/rol.h"
#include "instructions/opcodes.h"

#include <cstdint>
#include <iostream>

namespace avr {
    uint8_t& ROLInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void ROLInstruction::SetRegisterFlags(CPU& cpu, uint8_t original, uint8_t result) const
    {
        cpu.SREG.H = (result & 0x08u) != 0u;
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = (original & 0x80u) != 0u;
        cpu.SREG.V = cpu.SREG.N ^ cpu.SREG.C;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }

    uint32_t ROLInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        _clock.ConsumeCycle();
        auto originalValue = rd;
        rd = rd << 1;
        SetRegisterFlags(ctx.cpu, originalValue, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool ROLInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::ROL);
        auto mask = static_cast<uint16_t>(OpCodeMask::ROL);
        if ((opcode & mask) != op)
            return false;

        auto destMask = 0x01F0u;
        uint8_t dest = static_cast<uint8_t>((opcode >> 4) & (destMask >> 4));
        auto srcMask = 0x020Fu;
        uint8_t src = static_cast<uint8_t>((opcode & (srcMask & 0xFu)) | ((srcMask >> 8) & (opcode >> 8)));

        return dest == src;
    }
}
