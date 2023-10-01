#include "instructions/sbis.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint8_t SBISInstruction::GetBit(uint16_t opcode) const
    {
        return static_cast<uint8_t>(opcode & 0x07u);
    }

    uint8_t& SBISInstruction::GetIORegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x00F8u;
        auto index = static_cast<uint8_t>((opcode & mask) >> 3);
        return cpu.GPIO[index];
    }

    uint16_t SBISInstruction::GetNextOpCode(const CPU& cpu, const ProgramMemory& mem) const
    {
        auto pc = cpu.PC;
        auto opcode = static_cast<uint16_t>(0u);
        for (uint16_t i = 0u; i < sizeof(pc); i++)
            opcode |= static_cast<uint16_t>(mem[pc++] << (i * 8u));
        return opcode;
    }

    uint16_t SBISInstruction::GetOpCodeSize(uint16_t opcode) const
    {
        const static std::array twoWordOpCodes = 
        {
            std::make_tuple<OpCode, OpCodeMask>(OpCode::CALL, OpCodeMask::CALL),
            std::make_tuple<OpCode, OpCodeMask>(OpCode::JMP, OpCodeMask::JMP),
            std::make_tuple<OpCode, OpCodeMask>(OpCode::LDS, OpCodeMask::LDS),
            std::make_tuple<OpCode, OpCodeMask>(OpCode::STS, OpCodeMask::STS),
        };

        auto it = std::find_if(std::begin(twoWordOpCodes), std::end(twoWordOpCodes),
                [=] (const auto& item)
                {
                    const auto& [op, mask] = item;
                    return (opcode & static_cast<uint16_t>(mask)) == static_cast<uint16_t>(op);
                }
        );

        return (it == std::end(twoWordOpCodes)) ? 1u : 2u;
    }

    uint32_t SBISInstruction::Execute(uint16_t opcode, ExecutionContext& ctx) const
    {
        auto bit = GetBit(opcode);
        auto& io = GetIORegister(ctx.cpu, opcode);

        auto bitIsSet = (io & (0x1u << bit)) != 0u;
        if (!bitIsSet) {
            _clock.ConsumeCycle();
            return 1u;
        }

        auto nextOpcode = GetNextOpCode(ctx.cpu, ctx.progMem);
        auto nextOpcodeSize = GetOpCodeSize(nextOpcode);

        for (uint16_t i = 0; i < nextOpcodeSize; i++)
            _clock.ConsumeCycle();

        ctx.cpu.PC += static_cast<uint16_t>(nextOpcodeSize * 2u);

        return 1u + nextOpcodeSize;
    }

    bool SBISInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::SBIS);
        auto mask = static_cast<uint16_t>(OpCodeMask::SBIS);
        return (opcode & mask) == op;
    }
}
