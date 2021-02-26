#pragma once


#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class FMULInstruction: public InstructionExecutor {
        private:
            IClock& _clock;

            bool Matches(uint16_t opcode, OpCode code, OpCodeMask mask) const;
            uint8_t& GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;
            void WriteResult(CPU& cpu, uint16_t result) const;
            void SetRegisterFlags(CPU& cpu, uint16_t result, bool carry) const;

        public:
            FMULInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
