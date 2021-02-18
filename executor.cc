#include "executor.h"

#include "cpu.h"
#include "instructions/instructionexecutor.h"
#include "memory.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace avr {
    uint16_t Executor::FetchWord(const Memory& memory, const uint16_t address) const
    {
        uint16_t value = 0u;
        value = memory[address];
        value |= static_cast<uint16_t>((memory[address+1] << 8) & 0xFF00u);
        return value;
    }


    const std::unique_ptr<InstructionExecutor>& Executor::GetExecutor(const uint16_t opcode) const
    {
        auto it = std::find_if(
            std::begin(_executors),
            std::end(_executors),
            [opcode] (const auto& executor) { return executor->Matches(opcode); });
        return *it;
    }

    void Executor::Execute(CPU& cpu, Memory& memory, uint32_t cyclesRequested) const {
        using namespace std::string_literals;
        auto cyclesConsumed = 0u;

        while (cyclesConsumed < cyclesRequested)
        {
            auto opcode = FetchWord(memory, cpu.PC);
            cpu.PC += sizeof(cpu.PC);
            const auto& instruction_executor = GetExecutor(opcode);
            cyclesConsumed += instruction_executor->Execute(opcode, cpu, memory);
        }
    }
}