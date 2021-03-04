#include "instructions/muls.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& MULSInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x0Fu;
        uint8_t value = static_cast<uint8_t>((opcode & mask) + 16u);
        return cpu.R[value];
    }

    uint8_t& MULSInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0xF0u;
        uint8_t value = static_cast<uint8_t>(((opcode >> 4) & (mask >> 4)) + 16u);
        return cpu.R[value];
    }

    void MULSInstruction::SetRegisterFlags(CPU& cpu, int16_t result) const
    {
        cpu.SREG.C = (result & 0x8000) != 0;
        cpu.SREG.Z = result == 0;
    }

    uint32_t MULSInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        int16_t result = static_cast<int8_t>(rr) * static_cast<int8_t>(rd);
        SetRegisterFlags(ctx.cpu, result);
        for (auto i = 0u; i < sizeof(result); i++)
            ctx.cpu.R[i] = static_cast<uint8_t>((result >> (i * 8)) & 0xFF);
        
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool MULSInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::MULS);
        auto mask = static_cast<uint16_t>(OpCodeMask::MULS);
        return (opcode & mask) == op;
    }
}
