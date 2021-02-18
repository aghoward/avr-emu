#include "instructions/add.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& ADDInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x020F;
        uint8_t value = static_cast<uint8_t>((opcode & (mask & 0xFF)) | ((opcode >> 5) & (mask >> 5)));
        return cpu.R[value];
    }

    uint8_t& ADDInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0;
        uint8_t value = static_cast<uint8_t>((opcode >> 4) & (mask >> 4));
        return cpu.R[value];
    }

    void ADDInstruction::SetRegisterFlags(CPU& cpu, uint8_t& rr, uint8_t& rd, uint8_t result) const
    {
        cpu.SREG.H = ((rd & 0x08) && (rr & 0x08)) ||
            (((rr & 0x08) || (rd & 0x08)) && !(result & 0x08));
        cpu.SREG.V = ((rd & 0x80) && (rr & 0x80) && !(result & 0x80)) ||
            (!(rd & 0x80) && !(rr & 0x80) && (result & 0x80));
        cpu.SREG.N = static_cast<bool>(result & 0x80);
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
        cpu.SREG.Z = result == 0;
        cpu.SREG.C = ((rd & 0x80) && (rr & 0x80)) || 
            (!(result & 0x80) && ((rd & 0x80) || (rr & 0x80)));
    }

    uint32_t ADDInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto& rr = GetSourceRegister(cpu, opcode);
        auto& rd = GetDestinationRegister(cpu, opcode);

        auto value = static_cast<uint8_t>(rr + rd);
        if (IsADC(opcode))
            value += cpu.SREG.C;

        SetRegisterFlags(cpu, rr, rd, value);
        rd = value;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool ADDInstruction::IsADC(uint16_t opcode) const
    {
        auto adc = static_cast<uint16_t>(OpCode::ADC);
        auto mask = static_cast<uint16_t>(OpCodeMask::ADC);
        return (opcode & mask) == adc;
    }

    bool ADDInstruction::IsADD(uint16_t opcode) const
    {
        auto add = static_cast<uint16_t>(OpCode::ADD);
        auto mask = static_cast<uint16_t>(OpCodeMask::ADD);
        return (opcode & mask) == add;
    }

    bool ADDInstruction::Matches(uint16_t opcode) const
    {
        return IsADC(opcode) || IsADD(opcode);
    }
}
