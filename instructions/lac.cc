#include "instructions/lac.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LACInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LACInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM& mem) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);

        auto z = mem[*cpu.Z];
        mem[*cpu.Z] &= static_cast<uint8_t>(0xffu - rd);
        _clock.ConsumeCycle();
        rd = z;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LACInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LAC);
        auto mask = static_cast<uint16_t>(OpCodeMask::LAC);
        return (opcode & mask) == op;
    }
}
