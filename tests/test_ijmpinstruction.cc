#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/ijmp.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class IJMPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        IJMPInstruction subject;
        SRAM memory;
        CPU cpu;

        uint16_t InitializeSP()
        {
            uint16_t sp = static_cast<uint16_t>(rand()) + 2u + cpu.SRAM_BEG;
            cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(cpu.PC); i++)
                memory[i] = 0u;
            return sp;
        }

    public:
        IJMPInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(IJMPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(IJMPInstructionTests, Matches_GivenIJMPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::IJMP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(IJMPInstructionTests, Execute_JumpsToOperand)
{
    auto opcode = static_cast<uint16_t>(OpCode::IJMP);
    auto address = static_cast<uint16_t>(
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_RAM_SIZE) & 0xFFFEu);
    cpu.Z = address;
    auto originalSP = InitializeSP();

    auto cycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cycles, 2u);
    ASSERT_EQ(cpu.PC, address);
    ASSERT_EQ(cpu.SP, originalSP);
}
