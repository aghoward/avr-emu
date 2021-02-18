#pragma once


#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class CALLInstruction: public InstructionExecutor {
        private:
            const uint32_t _cyclesConsumed = 4u;

            void PushReturnAddress(CPU& cpu, Memory& mem) const;
            uint16_t GetDestinationAddress(CPU& cpu, Memory& mem) const;

        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
