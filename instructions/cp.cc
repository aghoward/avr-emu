#include "instructions/cp.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& CPInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint8_t& CPInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto index = static_cast<uint16_t>(
                ((opcode & 0x200u) >> 5u) |
                (opcode & 0x0Fu));
        return cpu.R[index];
    }

    void CPInstruction::SetStatusRegisters(CPU& cpu, uint8_t rr, uint8_t rd, int8_t result) const
    {
        cpu.SREG.Z = result == 0;
        cpu.SREG.N = result < 0;
        cpu.SREG.C = (!(rd & 0x80u) && (rr & 0x80u))
            || ((rr & 0x80u) && (result & 0x80))
            || ((result & 0x80) && !(rd & 0x80u));
        cpu.SREG.V = ((rd & 0x80u) && !(rr & 0x80u) && !(result & 0x80))
            || (!(rd & 0x80u) && (rr & 0x80u) && (result & 0x80));
        cpu.SREG.S = cpu.SREG.V ^ cpu.SREG.N;
        cpu.SREG.H = (!(rd & 0x08u) && (rr & 0x08u))
            || ((rr & 0x08u) && (result & 0x08))
            || ((result & 0x08) && !(rd & 0x08u));
    }

    uint32_t CPInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);
        auto& rr = GetSourceRegister(cpu, opcode);

        int8_t value = static_cast<int8_t>(rd) - static_cast<int8_t>(rr);
        if (Matches(opcode, OpCode::CPC, OpCodeMask::CPC))
            value -= cpu.SREG.C;

        SetStatusRegisters(cpu, rr, rd, value);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool CPInstruction::Matches(uint16_t opcode, OpCode op, OpCodeMask mask) const
    {
        return (opcode & static_cast<uint16_t>(mask)) == static_cast<uint16_t>(op);
    }

    bool CPInstruction::Matches(uint16_t opcode) const
    {
        return Matches(opcode, OpCode::CP, OpCodeMask::CP) ||
            Matches(opcode, OpCode::CPC, OpCodeMask::CPC);
    }
}
