#pragma once

#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class CBIInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 2u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t GetSourceBit(uint16_t opcode) const;

        public:
            CBIInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
