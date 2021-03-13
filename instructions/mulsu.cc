#include "instructions/mulsu.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& MULSUInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x07u;
        uint8_t value = static_cast<uint8_t>((opcode & mask) + 16u);
        return cpu.R[value];
    }

    uint8_t& MULSUInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x70u;
        uint8_t value = static_cast<uint8_t>(((opcode >> 4) & (mask >> 4)) + 16u);
        return cpu.R[value];
    }

    void MULSUInstruction::SetRegisterFlags(CPU& cpu, int16_t result) const
    {
        cpu.SREG.C = (result & 0x8000) != 0;
        cpu.SREG.Z = result == 0;
    }

    uint32_t MULSUInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        auto result = static_cast<int16_t>(static_cast<uint8_t>(rr) * static_cast<int8_t>(rd));
        SetRegisterFlags(ctx.cpu, result);
        for (auto i = 0u; i < sizeof(result); i++)
            ctx.cpu.R[i] = static_cast<uint8_t>((result >> (i * 8)) & 0xFF);
        
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool MULSUInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::MULSU);
        auto mask = static_cast<uint16_t>(OpCodeMask::MULSU);
        return (opcode & mask) == op;
    }
}
