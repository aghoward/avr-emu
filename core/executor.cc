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

    void Executor::Execute(ExecutionContext& ctx, uint32_t cyclesRequested) const {
        using namespace std::string_literals;
        auto cyclesConsumed = 0u;

        while (cyclesConsumed < cyclesRequested && !ctx.cpu.is_sleeping)
        {
            auto opcode = FetchWord(ctx.progMem, ctx.cpu.PC);
            ctx.cpu.PC += sizeof(ctx.cpu.PC);
            const auto& instruction_executor = GetExecutor(opcode);
            cyclesConsumed += instruction_executor->Execute(opcode, ctx);
        }
    }

    void Executor::Interrupt(ExecutionContext& ctx, uint8_t interrupt) const
    {
        ctx.cpu.R[24] = interrupt;
        auto old_pc = ctx.cpu.PC;
        // push PC
        ctx.ram[ctx.cpu.SP--] = (ctx.cpu.PC & 0xff);
        ctx.ram[ctx.cpu.SP--] = ((ctx.cpu.PC >> 8) & 0xff);
        ctx.cpu.PC = 0x912;

        while (ctx.cpu.PC != old_pc)
            Execute(ctx, 1);
    }
}
