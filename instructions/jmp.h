#pragma once

#include "core/iclock.h"
#include "instructions/instructionexecutor.h"

#include <cstdint>

namespace avr {
    class JMPInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 3u;

            uint16_t GetDestinationAddress(CPU& cpu, SRAM& mem) const;

        public:
            JMPInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, CPU& cpu, SRAM& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
