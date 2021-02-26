#include "core/cpu.h"
#include "core/memory.h"
#include "instructions/andi.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    uint8_t ANDIInstruction::GetSourceValue(uint16_t opcode) const
    {
        auto highNibble = static_cast<uint8_t>((opcode & 0x0F00u) >> 4);
        auto lowNibble = static_cast<uint8_t>(opcode & 0x000Fu);
        return highNibble | lowNibble;
    }

    uint8_t& ANDIInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto dstIndex = static_cast<uint8_t>(((opcode & 0x00f0) >> 4) + 16);
        return cpu.R[dstIndex];
    }

    void ANDIInstruction::SetRegisterFlags(CPU& cpu, uint16_t result) const
    {
        cpu.SREG.V = 0;
        cpu.SREG.N = (result & 0x80) != 0u;
        cpu.SREG.Z = result == 0;
    }

    uint32_t ANDIInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto src = GetSourceValue(opcode);
        auto& dst = GetDestinationRegister(ctx.cpu, opcode);
        
        dst = static_cast<uint16_t>(dst & src);
        SetRegisterFlags(ctx.cpu, dst);
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool ANDIInstruction::Matches(uint16_t opcode) const
    {
        const auto andi = static_cast<uint16_t>(OpCode::ANDI);
        auto mask = static_cast<uint16_t>(OpCodeMask::ANDI);
        return (opcode & mask) == andi;
    }
}
