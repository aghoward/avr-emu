#include "instructions/call.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    void CALLInstruction::PushReturnAddress(CPU& cpu, Memory& mem) const
    {
        uint16_t returnAddress = cpu.PC + static_cast<uint16_t>(sizeof(cpu.PC));
        for (uint16_t i = 0u; i < sizeof(cpu.PC); i++)
            mem[cpu.SP--] = static_cast<uint8_t>((returnAddress >> (i * 8u)) & 0xFFu);
    }

    uint16_t CALLInstruction::GetDestinationAddress(CPU& cpu, Memory& mem) const
    {
        auto address = static_cast<uint16_t>(0u);
        for (auto i = 0u; i < sizeof(address); i++)
            address |= static_cast<uint16_t>(mem[cpu.PC++] << (i * 8u));
        // Actual avr assembly has this shifted right one bit to prevent odd addresses
        return (address << 1u);
    }

    uint32_t CALLInstruction::Execute(uint16_t, CPU& cpu, Memory& mem) const
    {
        PushReturnAddress(cpu, mem);
        cpu.PC = GetDestinationAddress(cpu, mem);
        return _cyclesConsumed;
    }

    bool CALLInstruction::Matches(uint16_t opcode) const
    {
        const auto op = static_cast<uint16_t>(OpCode::CALL);
        auto mask = static_cast<uint16_t>(OpCodeMask::CALL);
        return (opcode & mask) == op;
    }
}
