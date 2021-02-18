#pragma once


#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class BREAKInstruction: public InstructionExecutor {
        private:
            const uint16_t _cyclesConsumed = 1u;

        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
