#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class LDDInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t GetDisplacement(uint16_t opcode) const;

            bool IsPostIncrement(uint16_t opcode) const;
            bool IsPreDecrement(uint16_t opcode) const;
            bool IsDisplaced(uint16_t opcode) const;

        public:
            LDDInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
