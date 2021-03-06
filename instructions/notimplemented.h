#pragma once


#include "core/executioncontext.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class NotImplementedInstruction : public InstructionExecutor {
        public:
            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
