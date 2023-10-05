#include "instructions/swap.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint8_t& SWAPInstruction::GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint8_t>((opcode & mask) >> 4u);
        return ctx.cpu.R[index];
    }

    uint32_t SWAPInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& source = GetSourceRegister(opcode, ctx);
        auto lowNibble = source & 0x0Fu;
        source = (lowNibble << 4u) | (source >> 4u);
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool SWAPInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::SWAP);
        auto mask = static_cast<uint16_t>(OpCodeMask::SWAP);
        return (opcode & mask) == op;
    }
}
