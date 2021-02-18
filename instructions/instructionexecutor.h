#pragma once

#include "core/cpu.h"
#include "core/memory.h"

#include <cstdint>

namespace avr {
    class InstructionExecutor {
        public:
            virtual ~InstructionExecutor() {}
            virtual uint32_t Execute(uint16_t opcode, CPU& cpu, Memory& memory) const = 0;
            virtual bool Matches(uint16_t opcode) const = 0;
    };
}
