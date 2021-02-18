#pragma once

#include "cpu.h"
#include "instructions/instructionexecutor.h"
#include "memory.h"

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
            void Execute(CPU& cpu, Memory& memory, uint32_t cyclesRequested) const;
    };
}
