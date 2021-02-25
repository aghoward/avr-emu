#include "instructions/in.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& INInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x060F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.GPIO[value];
    }

    uint8_t& INInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    uint32_t INInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM&) const
    {
        auto& rr = GetSourceRegister(cpu, opcode);
        auto& rd = GetDestinationRegister(cpu, opcode);

        rd = rr;

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool INInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::IN);
        auto mask = static_cast<uint16_t>(OpCodeMask::IN);
        return (opcode & mask) == op;
    }
}
