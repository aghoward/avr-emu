#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class ROLInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            void SetRegisterFlags(CPU& cpu, uint8_t original, uint8_t result) const;

        public:
            ROLInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
