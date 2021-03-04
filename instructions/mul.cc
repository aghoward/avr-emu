#include "instructions/mul.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& MULInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x020F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.R[value];
    }

    uint8_t& MULInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void MULInstruction::SetRegisterFlags(CPU& cpu, uint16_t result) const
    {
        cpu.SREG.C = (result & 0x8000u) != 0u;
        cpu.SREG.Z = result == 0u;
    }

    uint32_t MULInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        uint16_t result = static_cast<uint16_t>(rr) * static_cast<uint16_t>(rd);
        SetRegisterFlags(ctx.cpu, result);
        for (auto i = 0u; i < sizeof(result); i++)
            ctx.cpu.R[i] = static_cast<uint8_t>((result >> (i * 8u)) & 0xFF);
        
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool MULInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::MUL);
        auto mask = static_cast<uint16_t>(OpCodeMask::MUL);
        return (opcode & mask) == op;
    }
}
