#include "instructions/fmul.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& FMULInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x07u;
        uint8_t value = static_cast<uint8_t>(opcode & mask);
        return cpu.R[0x10u + value];
    }

    uint8_t& FMULInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x0070;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[0x10u + value];
    }

    void FMULInstruction::WriteResult(CPU& cpu, uint16_t result) const
    {
        for (auto i = 0u; i < sizeof(result); i++)
            cpu.R[i] = (result >> (8u * i)) & 0xFFu;
    }

    void FMULInstruction::SetRegisterFlags(CPU& cpu, uint16_t result, bool carry) const
    {
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = carry;
    }

    uint32_t FMULInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rr = GetSourceRegister(ctx.cpu, opcode);
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        auto result = static_cast<uint16_t>(static_cast<uint16_t>(rr) * static_cast<uint16_t>(rd));
        auto carry = (0x8000u & result) != 0u;
        result = result << 1u;

        SetRegisterFlags(ctx.cpu, result, carry);

        _clock.ConsumeCycle();
        WriteResult(ctx.cpu, result);
        _clock.ConsumeCycle();

        return 2u;
    }

    bool FMULInstruction::Matches(uint16_t opcode, OpCode code, OpCodeMask mask) const
    {
        return (opcode & static_cast<uint16_t>(mask)) == static_cast<uint16_t>(code);
    }

    bool FMULInstruction::Matches(uint16_t opcode) const
    {
        return
            Matches(opcode, OpCode::FMUL, OpCodeMask::FMUL) ||
            Matches(opcode, OpCode::FMULS, OpCodeMask::FMULS) ||
            Matches(opcode, OpCode::FMULSU, OpCodeMask::FMULSU);
    }
}
