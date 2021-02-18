#pragma once

#include "core/cpu.h"
#include "core/memory.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace avr {
    class Executor {
        private:
            std::vector<std::unique_ptr<InstructionExecutor>> _executors;

            uint16_t FetchWord(const Memory& memory, const uint16_t address) const;
            const std::unique_ptr<InstructionExecutor>& GetExecutor(const uint16_t opcode) const;

        public:
            Executor(std::vector<std::unique_ptr<InstructionExecutor>>&& executors)
                : _executors(std::move(executors))
            {}

            void Execute(CPU& cpu, Memory& memory, uint32_t cyclesRequested) const;
    };
}
