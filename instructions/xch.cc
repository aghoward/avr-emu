#include "instructions/xch.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint8_t& XCHInstruction::GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint8_t>((opcode & mask) >> 4u);
        return ctx.cpu.R[index];
    }

    uint32_t XCHInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& source = GetSourceRegister(opcode, ctx);
        auto newSourceValue = ctx.ram[*ctx.cpu.Z];
        ctx.ram[*ctx.cpu.Z] = source;
        _clock.ConsumeCycle();
        source = newSourceValue;
        _clock.ConsumeCycle();
        return _cyclesConsumed;
    }

    bool XCHInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::XCH);
        auto mask = static_cast<uint16_t>(OpCodeMask::XCH);
        return (opcode & mask) == op;
    }
}
