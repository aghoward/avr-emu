#pragma once


#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class ASRInstruction: public InstructionExecutor {
        private:
            const uint32_t _cyclesConsumed = 1u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            void SetRegisterFlags(CPU& cpu, uint8_t rd, uint8_t result) const;

        public:
            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
