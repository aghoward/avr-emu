#include "instructions/sbiw.h"
#include "core/cpu.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t SBIWInstruction::GetImmediate(uint16_t opcode) const
    {
        return static_cast<uint8_t>(
            ((opcode & 0xC0u) >> 2u) |
            (opcode & 0xFu));
    }

    IndirectRegister SBIWInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x0030u;
        uint8_t index = (static_cast<uint8_t>((opcode & mask) >> 4) * 2u) + 24u;
        return IndirectRegister(&cpu.R[index]);
    }

    void SBIWInstruction::SetRegisterFlags(CPU& cpu, uint8_t k, uint16_t original, uint16_t result) const
    {
        cpu.SREG.C = ((result & 0x8000u) != 0) && ((original & 0x8000u) == 0);
        cpu.SREG.Z = result == 0u;
        cpu.SREG.N = ((result & 0x8000u) != 0);
        cpu.SREG.V = ((result & 0x8000u) == 0) && ((original & 0x8000u) != 0);
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }
    

    uint32_t SBIWInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto rd = GetDestinationRegister(ctx.cpu, opcode);
        auto k = GetImmediate(opcode);
        auto originalValue = *rd;
        rd = rd - k;
        _clock.ConsumeCycle();
        SetRegisterFlags(ctx.cpu, k, originalValue, *rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool SBIWInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::SBIW);
        auto mask = static_cast<uint16_t>(OpCodeMask::SBIW);
        return (opcode & mask) == op;
    }
}
