#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/reti.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class RETIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        RETIInstruction subject;
        ExecutionContext ctx;

        void SetReturnAddress(uint16_t address)
        {
            for (auto i = static_cast<uint16_t>(0u); i < sizeof(address); i++)
                ctx.ram[ctx.cpu.SP--] = static_cast<uint8_t>((address >> (8u * i)) & 0xFFu);
        }

        uint16_t InitializeSP()
        {
            auto usableRamSize = static_cast<uint16_t>(AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG);
            uint16_t sp = static_cast<uint16_t>(rand() % usableRamSize) + 2u + ctx.cpu.SRAM_BEG;
            ctx.cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(ctx.cpu.PC); i++)
                ctx.ram[i] = 0u;
            return sp;
        }

        uint16_t GetExpectedReturnAddress()
        {
            auto address = static_cast<uint16_t>(
                static_cast<uint16_t>(rand()) % AVR_EMU_FLASH_SIZE);
            InitializeSP();
            SetReturnAddress(address);
            return address;
        }

    public:
        RETIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(RETIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(RETIInstructionTests, Matches_GivenRETIOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::RETI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(RETIInstructionTests, Execute_SetsPCToReturnAddress)
{
    auto expectedAddress = GetExpectedReturnAddress();
    auto expectedSP = ctx.cpu.SP + sizeof(ctx.cpu.PC);

    auto cycles = subject.Execute(static_cast<uint16_t>(OpCode::RETI), ctx);

    ASSERT_EQ(cycles, 4u);
    ASSERT_EQ(ctx.cpu.PC, expectedAddress);
    ASSERT_EQ(ctx.cpu.SP, expectedSP);
}

TEST_F(RETIInstructionTests, Execute_SetsInteruptFlag)
{
    ctx.cpu.SREG.I = 0;

    subject.Execute(static_cast<uint16_t>(OpCode::RETI), ctx);

    ASSERT_TRUE(ctx.cpu.SREG.I);
}
