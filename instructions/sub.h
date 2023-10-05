#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class SUBInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 1u;

            uint8_t& GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            void SetRegisterFlags(CPU& cpu, uint8_t source, uint8_t dest, uint8_t result) const;

        public:
            SUBInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
