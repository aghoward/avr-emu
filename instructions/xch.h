#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class XCHInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            uint8_t& GetSourceRegister(uint16_t opcode, const ExecutionContext& ctx) const;

        public:
            XCHInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
