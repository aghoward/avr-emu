#pragma once


#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class BCLRInstruction: public InstructionExecutor {
        private:
            const uint32_t _cyclesConsumed = 1u;

            uint8_t GetSource(uint16_t opcode) const;

        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
