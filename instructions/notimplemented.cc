#include "cpu.h"
#include "instructions/notimplemented.h"
#include "memory.h"

#include <cstdint>
#include <string>

namespace avr {
    uint32_t NotImplementedInstruction::Execute(uint16_t opcode, CPU&, Memory&) const
    {
        using namespace std::string_literals;
        throw "Instruction not implemented ("s + std::to_string(opcode) + ")"s;
        return 0;
    }

    bool NotImplementedInstruction::Matches(uint16_t) const
    {
        return true;
    }
}