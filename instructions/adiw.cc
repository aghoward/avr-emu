#include "core/cpu.h"
#include "core/memory.h"
#include "instructions/adiw.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t ADIWInstruction::GetSourceValue(uint16_t opcode) const
    {
        auto highNibble = static_cast<uint8_t>((opcode & 0x00C0u) >> 2);
        auto lowNibble = static_cast<uint8_t>(opcode & 0x000Fu);
        return highNibble | lowNibble;
    }

    uint16_t ADIWInstruction::GetDestinationRegister(uint16_t opcode) const
    {
        auto dstBits = static_cast<uint8_t>((opcode & 0x0030u) >> 4);
        return static_cast<uint16_t>(24u + (2u*dstBits));
    }

    void ADIWInstruction::SetRegisterFlags(CPU& cpu, uint8_t& rdh, uint16_t result) const
    {
        cpu.SREG.V = !(rdh & 0x80) && (result & 0x8000);
        cpu.SREG.N = static_cast<bool>(result & 0x8000u);
        cpu.SREG.S = static_cast<bool>(cpu.SREG.V ^ cpu.SREG.N);
        cpu.SREG.Z = result == 0;
        cpu.SREG.C = !(result & 0x8000) && (rdh & 0x80); 
    }

    uint32_t ADIWInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto src = GetSourceValue(opcode);
        auto dstIndex = GetDestinationRegister(opcode);
        
        auto value = static_cast<uint16_t>(cpu.R[dstIndex] | cpu.R[dstIndex+1] << 8u);
        value += src;

        SetRegisterFlags(cpu, cpu.R[dstIndex+1], value);

        cpu.R[dstIndex] = static_cast<uint8_t>(0xFF & value);
        cpu.R[dstIndex+1] = static_cast<uint8_t>(0xFF & (value >> 8));

        
        return _cyclesConsumed;
    }

    bool ADIWInstruction::Matches(uint16_t opcode) const
    {
        const auto adiw = static_cast<uint16_t>(OpCode::ADIW);
        const auto mask = static_cast<uint16_t>(OpCodeMask::ADIW);
        return (opcode & mask) == adiw;
    }
}
