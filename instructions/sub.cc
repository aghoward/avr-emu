#include "instructions/sub.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& SUBInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x020Fu;
        uint8_t value = static_cast<uint8_t>(((opcode >> 5) & (mask >> 5)) | (opcode & (mask & 0x0Fu)));
        return cpu.R[value];
    }

    uint8_t& SUBInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void SUBInstruction::SetRegisterFlags(CPU& cpu, uint8_t source, uint8_t dest, uint8_t result) const
    {
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = (result == 0u);
        cpu.SREG.C = (((dest & 0x80u) == 0u) && ((source & 0x80u) != 0u)) ||
            (((source & 0x80u) != 0u) && ((result & 0x80u) != 0u)) ||
            (((result & 0x80u) != 0u) && ((dest & 0x80u) == 0u));
        cpu.SREG.V = (((dest & 0x80u) != 0u) && ((source & 0x80u) == 0u) && ((result & 0x80u) == 0u)) ||
            (((dest & 0x80u) == 0u) && ((source & 0x80u) != 0u) && ((result & 0x80u) != 0u));
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
        cpu.SREG.H = (((dest & 0x08u) == 0u) && ((source & 0x08u) != 0u)) ||
            (((source & 0x08u) != 0u) && ((result & 0x08u) != 0u)) ||
            (((result & 0x08u) != 0u) && ((dest & 0x08u) == 0u));
    }

    uint32_t SUBInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto originalValue = rd;
        rd = rd - rr;
        SetRegisterFlags(ctx.cpu, rr, originalValue, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool SUBInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::SUB);
        auto mask = static_cast<uint16_t>(OpCodeMask::SUB);
        return (opcode & mask) == op;
    }
}
