#pragma once


#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class NotImplementedInstruction : public InstructionExecutor {
        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, SRAM& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
