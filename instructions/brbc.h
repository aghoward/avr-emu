#pragma once


#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class BRBCInstruction: public InstructionExecutor {
        private:
            uint8_t GetSourceValue(uint16_t opcode) const;
            uint8_t GetDestinationOffset(uint16_t opcode) const;
            bool ShouldBranch(uint16_t opcode, const CPU& cpu, uint8_t flagIndex) const;
            bool Matches(uint16_t opcode, OpCode op, OpCodeMask mask) const;

        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
