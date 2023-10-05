#include "instructions/sts.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint8_t& STSInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint8_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint16_t STSInstruction::GetAddress(const CPU& cpu, const ProgramMemory& mem) const
    {
        auto pc = cpu.PC;
        auto address = static_cast<uint16_t>(0u);
        for (uint16_t i = 0u; i < sizeof(pc); i++)
            address |= static_cast<uint16_t>(mem[pc++] << (i * 8u));
        return address;
    }

    uint32_t STSInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto& src = GetSourceRegister(ctx.cpu, opcode);
        auto address = GetAddress(ctx.cpu, ctx.progMem);
        _clock.ConsumeCycle();

        ctx.ram[address] = src;
        ctx.cpu.PC += 2u;
        _clock.ConsumeCycle();

        return _cyclesConsumed;
    }

    bool STSInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::STS);
        auto mask = static_cast<uint16_t>(OpCodeMask::STS);
        return (opcode & mask) == op;
    }
}
