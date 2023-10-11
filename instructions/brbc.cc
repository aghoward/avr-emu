#include "instructions/brbc.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t BRBCInstruction::GetSourceValue(uint16_t opcode) const
    {
        auto mask = 0x0007u;
        return static_cast<uint8_t>(opcode & mask);
    }

    int8_t BRBCInstruction::GetDestinationOffset(uint16_t opcode) const
    {
        auto mask = 0x03F8;
        return static_cast<uint8_t>((opcode & mask) >> 2);
    }

    bool BRBCInstruction::ShouldBranch(uint16_t opcode, const CPU& cpu, uint8_t flagIndex) const
    {
        auto branchIfSet = (opcode & 0x0400u) == 0u;

        if (flagIndex == 0u)
            return cpu.SREG.C == branchIfSet;
        if (flagIndex == 1u)
            return cpu.SREG.Z == branchIfSet;
        if (flagIndex == 2u)
            return cpu.SREG.N == branchIfSet;
        if (flagIndex == 3u)
            return cpu.SREG.V == branchIfSet;
        if (flagIndex == 4u)
            return cpu.SREG.S == branchIfSet;
        if (flagIndex == 5u)
            return cpu.SREG.H == branchIfSet;
        if (flagIndex == 6u)
            return cpu.SREG.T == branchIfSet;

        return cpu.SREG.I == branchIfSet;
    }

    uint32_t BRBCInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto flagIndex = GetSourceValue(opcode);

        auto shouldBranch = ShouldBranch(opcode, ctx.cpu, flagIndex);
        _clock.ConsumeCycle();
        if (!shouldBranch)
            return 1;

        auto offset = GetDestinationOffset(opcode);
        ctx.cpu.PC += offset;

        _clock.ConsumeCycle();

        return 2;
    }

    bool BRBCInstruction::Matches(uint16_t opcode, OpCode op, OpCodeMask mask) const
    {
        return (opcode & static_cast<uint16_t>(mask)) == static_cast<uint16_t>(op);
    }

    bool BRBCInstruction::Matches(uint16_t opcode) const
    {
        return Matches(opcode, OpCode::BRBC, OpCodeMask::BRBC) ||
            Matches(opcode, OpCode::BRBS, OpCodeMask::BRBS);
    }
}
