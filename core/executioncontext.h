#pragma once

#include "core/cpu.h"
#include "core/memory.h"

namespace avr {
    struct ExecutionContext {
        SRAM ram;
        ProgramMemory progMem;
        CPU cpu;

        ExecutionContext()
            : 
            ram(),
            progMem(),
            cpu(ram)
        {}
    };
}
