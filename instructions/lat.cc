#include "instructions/lat.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& LATInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint32_t LATInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM& mem) const
    {
        auto& rd = GetDestinationRegister(cpu, opcode);

        auto z = mem[*cpu.Z];
        mem[*cpu.Z] ^= rd;
        _clock.ConsumeCycle();
        rd = z;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool LATInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::LAT);
        auto mask = static_cast<uint16_t>(OpCodeMask::LAT);
        return (opcode & mask) == op;
    }
}
