#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class LPMInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 3u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;

            bool IsPostIncrement(uint16_t opcode) const;
            bool HasRegister(uint16_t opcode) const;

        public:
            LPMInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
