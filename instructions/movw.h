#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class MOVWInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 1u;

            uint8_t* GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t* GetDestinationRegister(CPU& cpu, uint16_t opcode) const;

        public:
            MOVWInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
