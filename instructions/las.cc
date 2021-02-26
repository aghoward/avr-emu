#include "instructions/las.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LASInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LASInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM& mem) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);

        auto z = mem[*cpu.Z];
        mem[*cpu.Z] |= rd;
        _clock.ConsumeCycle();
        rd = z;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LASInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LAS);
        auto mask = static_cast<uint16_t>(OpCodeMask::LAS);
        return (opcode & mask) == op;
    }
}
