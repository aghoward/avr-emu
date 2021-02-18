#include "instructions/bst.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t BSTInstruction::GetSourceValue(uint16_t opcode) const
    {
        auto mask = 0x0007u;
        return static_cast<uint8_t>(opcode & mask);
    }

    uint8_t& BSTInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    uint32_t BSTInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto bitShift = GetSourceValue(opcode);
        auto& rd = GetDestinationRegister(cpu, opcode);

        cpu.SREG.T = (rd >> bitShift) == 1u;

        return _cyclesConsumed;
    }

    bool BSTInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::BST);
        auto mask = static_cast<uint16_t>(OpCodeMask::BST);
        return (opcode & mask) == op;
    }
}
