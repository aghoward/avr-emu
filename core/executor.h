#pragma once

#include "core/cpu.h"
#include "core/iclock.h"
#include "core/memory.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace avr {
    class Executor {
        private:
            IClock& _clock;
            std::vector<std::unique_ptr<InstructionExecutor>> _executors;

            uint16_t FetchWord(const ProgramMemory& progMem, const uint16_t address) const;
            const std::unique_ptr<InstructionExecutor>& GetExecutor(const uint16_t opcode) const;

        public:
            Executor(
                IClock& clock,
                std::vector<std::unique_ptr<InstructionExecutor>>&& executors)
                : _clock(clock),
                  _executors(std::move(executors))
            {}

            void Execute(ExecutionContext& ctx, uint32_t cyclesRequested) const;
            void Interrupt(ExecutionContext& ctx, uint8_t interrupt) const;
    };
}
