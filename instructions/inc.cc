#include "instructions/inc.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t& INCInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    void INCInstruction::SetStatusRegisters(CPU& cpu, uint8_t rd) const
    {
        cpu.SREG.Z = rd == 0;
        cpu.SREG.N = (rd & 0x80u) != 0u;
        cpu.SREG.V = rd == 0x80u;
        cpu.SREG.S = cpu.SREG.N ^ cpu.SREG.V;
    }

    uint32_t INCInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& rd = GetDestinationRegister(ctx.cpu, opcode);

        rd = static_cast<uint8_t>(rd + 1u);
        SetStatusRegisters(ctx.cpu, rd);

        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool INCInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::INC);
        auto mask = static_cast<uint16_t>(OpCodeMask::INC);
        return (opcode & mask) == op;
    }
}
