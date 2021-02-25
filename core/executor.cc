#include "core/cpu.h"
#include "core/executor.h"
#include "core/memory.h"
#include "instructions/instructionexecutor.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace avr {
    uint16_t Executor::FetchWord(const ProgramMemory& memory, const uint16_t address) const
    {
        uint16_t value = 0u;
        value = memory[address];
        value |= static_cast<uint16_t>((memory[address+1] << 8) & 0xFF00u);
        _clock.ConsumeCycle();
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

    void Executor::Execute(CPU& cpu, ProgramMemory& progMem, SRAM& sram, uint32_t cyclesRequested) const {
        using namespace std::string_literals;
        auto cyclesConsumed = 0u;

        while (cyclesConsumed < cyclesRequested)
        {
            auto opcode = FetchWord(progMem, cpu.PC);
            cpu.PC += sizeof(cpu.PC);
            const auto& instruction_executor = GetExecutor(opcode);
            cyclesConsumed += instruction_executor->Execute(opcode, cpu, sram);
        }
    }
}
