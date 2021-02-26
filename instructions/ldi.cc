#include "instructions/ldi.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t LDIInstruction::GetImmediate(uint16_t opcode) const
    {
        auto highNibble = (opcode & 0xF00u) >> 4u;
        auto lowNibble = (opcode & 0xFu);
        return static_cast<uint8_t>(highNibble | lowNibble);
    }

    uint8_t& LDIInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F0u;
        auto index = static_cast<uint16_t>(((opcode & mask) >> 4) + 16);
        return cpu.R[index];
    }

    uint32_t LDIInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);
        auto k = GetImmediate(opcode);

        rd = k;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LDIInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LDI);
        auto mask = static_cast<uint16_t>(OpCodeMask::LDI);
        return (opcode & mask) == op;
    }
}
