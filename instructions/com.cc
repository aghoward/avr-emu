#include "instructions/com.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& COMInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    void COMInstruction::SetStatusRegisters(CPU& cpu, uint8_t result) const
    {
        cpu.SREG.V = false;
        cpu.SREG.N = (result & 0x80u) != 0u;
        cpu.SREG.S = cpu.SREG.V ^ cpu.SREG.N;
        cpu.SREG.Z = result == 0u;
        cpu.SREG.C = true;

    }

    uint32_t COMInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM&) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);
        rd ^= 0xFFu;
        SetStatusRegisters(cpu, rd);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool COMInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::COM);
        auto mask = static_cast<uint16_t>(OpCodeMask::COM);
        return (opcode & mask) == op;
    }
}
