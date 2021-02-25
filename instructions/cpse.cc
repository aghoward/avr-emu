#include "instructions/cpse.h"
#include "instructions/opcodes.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <tuple>

namespace avr {
    uint8_t& CPSEInstruction::GetSourceRegister(CPU& cpu, uint16_t opcode) const
    {
        auto index = static_cast<uint16_t>(((opcode & 0x0200u) >> 5u) | (opcode & 0x000Fu));
        return cpu.R[index];
    }

    uint8_t& CPSEInstruction::GetDestinationRegister(CPU& cpu, uint16_t opcode) const
    {
        auto mask = 0x01F0u;
        auto index = static_cast<uint16_t>((opcode & mask) >> 4);
        return cpu.R[index];
    }

    uint16_t CPSEInstruction::GetNextOpCode(const CPU& cpu, const SRAM& mem) const
    {
        auto pc = cpu.PC;
        auto opcode = static_cast<uint16_t>(0u);
        for (uint16_t i = 0u; i < sizeof(pc); i++)
            opcode |= static_cast<uint16_t>(mem[pc++] << (i * 8u));
        return opcode;
    }

    uint16_t CPSEInstruction::GetOpCodeSize(uint16_t opcode) const
    {
        const static std::array twoWordOpCodes = 
        {
            std::make_tuple<OpCode, OpCodeMask>(OpCode::CALL, OpCodeMask::CALL),
            std::make_tuple<OpCode, OpCodeMask>(OpCode::JMP, OpCodeMask::JMP),
            std::make_tuple<OpCode, OpCodeMask>(OpCode::LDS, OpCodeMask::LDS),
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

    uint32_t CPSEInstruction::Execute(uint16_t opcode, CPU& cpu, SRAM& mem) const
    {
        auto& rr = GetSourceRegister(cpu, opcode);
        auto& rd = GetDestinationRegister(cpu, opcode);

        if (rr != rd)
        {
            _clock.ConsumeCycle();
            return 1u;
        }

        auto nextOpcode = GetNextOpCode(cpu, mem);
        auto nextOpcodeSize = GetOpCodeSize(nextOpcode);

        for (uint16_t i = 0; i < nextOpcodeSize; i++)
            _clock.ConsumeCycle();

        cpu.PC += static_cast<uint16_t>(nextOpcodeSize * 2u);

        return 1u + nextOpcodeSize;
    }

    bool CPSEInstruction::Matches(uint16_t opcode) const
    {
        auto op = static_cast<uint16_t>(OpCode::CPSE);
        auto mask = static_cast<uint16_t>(OpCodeMask::CPSE);
        return (opcode & mask) == op;
    }
}
