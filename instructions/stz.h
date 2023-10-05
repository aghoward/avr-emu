#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class STZInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            bool IsPreDecrement(uint16_t opcode) const;
            bool IsPostIncrement(uint16_t opcode) const;
            uint8_t GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const;
            uint8_t GetDisplacement(uint16_t opcode) const;

        public:
            STZInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
