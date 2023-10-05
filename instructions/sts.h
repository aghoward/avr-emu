#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class STSInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            uint8_t& GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            uint16_t GetAddress(const CPU& cpu, const ProgramMemory& mem) const;

        public:
            STSInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
