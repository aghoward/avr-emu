#include "instructions/dec.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& DECInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    void DECInstruction::SetStatusRegisters(CPU& cpu, uint8_t rd, int8_t result) const
    {
        cpu.SREG.Z = result == 0;
        cpu.SREG.N = (static_cast<uint8_t>(result) & 0x80u) != 0u;
        cpu.SREG.V = rd == 0x80u;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }

    uint32_t DECInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM&) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);

        int8_t value = static_cast<int8_t>(rd - 1u);
        SetStatusRegisters(cpu, rd, value);
        rd = static_cast<uint8_t>(value);

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool DECInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::DEC);
        auto mask = static_cast<uint16_t>(OpCodeMask::DEC);
        return (opcode & mask) == op;
    }
}
