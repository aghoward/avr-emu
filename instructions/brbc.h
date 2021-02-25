#pragma once

#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class BRBCInstruction: public InstructionExecutor {
        private:
            IClock& _clock;

            uint8_t GetSourceValue(uint16_t opcode) const;
            uint8_t GetDestinationOffset(uint16_t opcode) const;
            bool ShouldBranch(uint16_t opcode, const CPU& cpu, uint8_t flagIndex) const;
            bool Matches(uint16_t opcode, OpCode op, OpCodeMask mask) const;

        public:
            BRBCInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, CPU& cpu, SRAM& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
