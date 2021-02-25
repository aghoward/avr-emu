#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/icall.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class ICALLInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        ICALLInstruction subject;
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

        uint16_t ReadReturnAddress()
        {
            uint16_t address = 0u;
            uint16_t base_address = cpu.SP + 1u;
            for (uint16_t i = 0; i < sizeof(cpu.PC); i++)
                address |= static_cast<uint16_t>(memory[base_address + i] << ((sizeof(cpu.PC) - i - 1u) * 8u));
            return address;
        }

    public:
        ICALLInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ICALLInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ICALLInstructionTests, Matches_GivenICALLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ICALL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ICALLInstructionTests, Execute_LongJumpsToOperand)
{
    auto opcode = static_cast<uint16_t>(OpCode::ICALL);
    auto address = static_cast<uint16_t>(
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_FLASH_SIZE) & 0xFFFEu);
    cpu.Z = address;
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = cpu.PC;

    auto cycles = subject.Execute(opcode, cpu, memory);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 3u);
    ASSERT_EQ(cpu.PC, address);
    ASSERT_EQ(cpu.SP, originalSP - sizeof(cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}
