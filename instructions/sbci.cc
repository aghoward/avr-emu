#include "instructions/sbci.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t SBCIInstruction::GetImmediate(uint16_t opcode) const
    {
        return static_cast<uint8_t>(
            ((opcode >> 4u) & 0xF0u) |
            (opcode & 0xFu));
    }

    uint8_t& SBCIInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F0u;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4) | 0x10u);
        return cpu.R[value];
    }

    void SBCIInstruction::SetRegisterFlags(CPU& cpu, uint8_t k, uint8_t dest, uint8_t result) const
    {
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = (result == 0u) && cpu.SREG.Z;
        cpu.SREG.C = (((dest & 0x80u) == 0u) && ((k & 0x80u) != 0u)) ||
            (((k & 0x80u) != 0u) && ((result & 0x80u) != 0u)) ||
            (((result & 0x80u) != 0u) && ((dest & 0x80u) == 0u));
        cpu.SREG.V = (((dest & 0x80u) != 0u) && ((k & 0x80u) == 0u) && ((result & 0x80u) == 0u)) ||
            (((dest & 0x80u) == 0u) && ((k & 0x80u) != 0u) && ((result & 0x80u) != 0u));
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
        cpu.SREG.H = (((dest & 0x08u) == 0u) && ((k & 0x08u) != 0u)) ||
            (((k & 0x08u) != 0u) && ((result & 0x08u) != 0u)) ||
            (((result & 0x08u) != 0u) && ((dest & 0x08u) == 0u));
    }
    

    uint32_t SBCIInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto k = GetImmediate(opcode);
        auto originalValue = rd;
        rd = rd - k - ctx.cpu.SREG.C;
        SetRegisterFlags(ctx.cpu, k, originalValue, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool SBCIInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::SBCI);
        auto mask = static_cast<uint16_t>(OpCodeMask::SBCI);
        return (opcode & mask) == op;
    }
}
