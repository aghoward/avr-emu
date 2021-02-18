#pragma once

#include "core/iclock.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class ANDIInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 1u;

            uint8_t GetSourceValue(uint16_t opcode) const;
            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            void SetRegisterFlags(CPU& cpu, uint16_t result) const;

        public:
            ANDIInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
