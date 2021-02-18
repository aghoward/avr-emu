#include "instructions/asr.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& ASRInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void ASRInstruction::SetRegisterFlags(CPU& cpu, uint8_t rd, uint8_t result) const
    {
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = (rd & 0x1) == 1u;
        cpu.SREG.V = cpu.SREG.N ^ cpu.SREG.C;
        cpu.SREG.S = cpu.SREG.V ^ cpu.SREG.N;
    }

    uint32_t ASRInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);
        bool signBit = (rd & 0x80) != 0u;
        auto value = static_cast<uint8_t>((rd >> 1) | (signBit << 7));

        SetRegisterFlags(cpu, rd, value);
        rd = value;

        return _cyclesConsumed;
    }

    bool ASRInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::ASR);
        auto mask = static_cast<uint16_t>(OpCodeMask::ASR);
        return (opcode & mask) == op;
    }
}
