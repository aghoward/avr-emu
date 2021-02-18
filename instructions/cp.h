#pragma once

#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class CPInstruction: public InstructionExecutor {
        private:
            IClock& _clock;
            const uint32_t _cyclesConsumed = 1u;

            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t& GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            void SetStatusRegisters(CPU& cpu, uint8_t rr, uint8_t rd, int8_t result) const;
            bool Matches(uint16_t opcode, OpCode op, OpCodeMask mask) const;

        public:
            CPInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
