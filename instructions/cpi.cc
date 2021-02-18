#include "instructions/cpi.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& CPIInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F0u;
        auto index = static_cast<uint16_t>(((opcode & mask) >> 4) + 0x10u);
        return cpu.R[index];
    }

    uint8_t CPIInstruction::GetImmediateValue(uint16_t opcode) const
    {
        return static_cast<uint8_t>((0x0F00u & opcode) >> 4u)
            | static_cast<uint8_t>(0xFu & opcode);
    }

    void CPIInstruction::SetStatusRegisters(CPU& cpu, uint8_t k, uint8_t rd, int8_t result) const
    {
        cpu.SREG.Z = result == 0;
        cpu.SREG.N = result < 0;
        cpu.SREG.C = (!(rd & 0x80u) && (k & 0x80u))
            || ((k & 0x80u) && (result & 0x80))
            || ((result & 0x80) && !(rd & 0x80u));
        cpu.SREG.V = ((rd & 0x80u) && !(k & 0x80u) && !(result & 0x80))
            || (!(rd & 0x80u) && (k & 0x80u) && (result & 0x80));
        cpu.SREG.S = cpu.SREG.V ^ cpu.SREG.N;
        cpu.SREG.H = (!(rd & 0x08u) && (k & 0x08u))
            || ((k & 0x08u) && (result & 0x08))
            || ((result & 0x08) && !(rd & 0x08u));
    }

    uint32_t CPIInstruction::Execute(uint16_t opcode, CPU& cpu, Memory&) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);
        auto k = GetImmediateValue(opcode);

        int8_t value = static_cast<int8_t>(rd) - static_cast<int8_t>(k);

        SetStatusRegisters(cpu, k, rd, value);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool CPIInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::CPI);
        auto mask = static_cast<uint16_t>(OpCodeMask::CPI);
        return (opcode & mask) == op;
    }
}
