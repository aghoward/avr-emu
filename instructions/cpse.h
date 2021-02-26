#pragma once

#include "core/executioncontext.h"
#include "core/iclock.h"
#include "instructions/instructionexecutor.h"
#include "instructions/opcodes.h"

#include <cstdint>

namespace avr {
    class CPSEInstruction: public InstructionExecutor {
        private:
            IClock& _clock;

            uint8_t& GetSourceRegister(CPU& cpu, uint16_t opcode) const;
            uint8_t& GetDestinationRegister(CPU& cpu, uint16_t opcode) const;

            uint16_t GetNextOpCode(const CPU& cpu, const ProgramMemory& mem) const;
            uint16_t GetOpCodeSize(uint16_t opcode) const;

        public:
            CPSEInstruction(IClock& clock)
                : _clock(clock)
            {}

            uint32_t Execute(uint16_t opcode, ExecutionContext& ctx) const override;
            bool Matches(uint16_t opcode) const override;
    };
}
