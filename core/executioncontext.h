#pragma once

#include "core/cpu.h"
#include "core/memory.h"

namespace avr {
    struct ExecutionContext {
        private:
            std::shared_ptr<Memory> _ram;
            std::shared_ptr<Memory> _progMem;

        public:
            Memory& ram;
            Memory& progMem;
            CPU cpu;

        ExecutionContext()
            : 
            _ram(std::make_shared<Memory>(AVR_EMU_RAM_SIZE)),
            _progMem(std::make_shared<Memory>(AVR_EMU_FLASH_SIZE)),
            ram(*_ram),
            progMem(*_progMem),
            cpu(ram)
        {}

        ExecutionContext(
            const std::shared_ptr<Memory>& ram_memory,
            const std::shared_ptr<Memory>& prog_memory
        ) :
            _ram(ram_memory),
            _progMem(prog_memory),
            ram(*_ram),
            progMem(*_progMem),
            cpu(ram)
        {}
    };
}
