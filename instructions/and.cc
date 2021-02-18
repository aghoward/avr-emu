#include "instructions/and.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& ANDInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x020F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.R[value];
    }

    uint8_t& ANDInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void ANDInstruction::SetRegisterFlags(CPU& cpu, uint8_t result) const
    {
        cpu.SREG.V = 0;
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.S = cpu.SREG.V ^ cpu.SREG.N;
        cpu.SREG.Z = result == 0u;
    }

    uint32_t ANDInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto& rr = GetSourceRegister(cpu, opcode);
        auto& rd = GetDestinationRegister(cpu, opcode);

        auto value = static_cast<uint8_t>(rr & rd);

        SetRegisterFlags(cpu, value);
        rd = value;

        return _cyclesConsumed;
    }

    bool ANDInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::AND);
        auto mask = static_cast<uint16_t>(OpCodeMask::AND);
        return (opcode & mask) == op;
    }
}
