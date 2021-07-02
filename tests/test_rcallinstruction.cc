#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/rcall.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class RCALLInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        RCALLInstruction subject;
        ExecutionContext ctx;

        uint16_t InitializeSP()
        {
            uint16_t sp = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + 2u + ctx.cpu.SRAM_BEG;
            ctx.cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(ctx.cpu.PC); i++)
                ctx.ram[sp - i] = 0u;
            return sp;
        }

        uint16_t ReadReturnAddress()
        {
            uint16_t address = 0u;
            uint16_t base_address = ctx.cpu.SP + 1u;
            for (uint16_t i = 0; i < sizeof(ctx.cpu.PC); i++)
                address |= static_cast<uint16_t>(ctx.ram[base_address + i] << ((sizeof(ctx.cpu.PC) - i - 1u) * 8u));
            return address;
        }

        uint16_t GetOpCode(int16_t relativePosition)
        {
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::RCALL) |
                (relativePosition & 0x0FFF));
        }

    public:
        RCALLInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(RCALLInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(RCALLInstructionTests, Matches_GivenRCALLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::RCALL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(RCALLInstructionTests, Execute_GivenPositiveOffset_JumpsForward)
{
    auto address = static_cast<int16_t>(
            static_cast<int16_t>(rand() & 0x07FF) % AVR_EMU_FLASH_SIZE & 0xFFFEu);
    auto opcode = GetOpCode(address);
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = ctx.cpu.PC;
    auto expectedNewAddress = static_cast<uint16_t>(ctx.cpu.PC + address);

    auto cycles = subject.Execute(opcode, ctx);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 3u);
    ASSERT_EQ(ctx.cpu.PC, expectedNewAddress);
    ASSERT_EQ(ctx.cpu.SP, originalSP - sizeof(ctx.cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}

TEST_F(RCALLInstructionTests, Execute_GivenNegativeOffset_JumpsBackward)
{
    auto address = static_cast<int16_t>(
            static_cast<int16_t>(rand() & 0x07FF) % AVR_EMU_FLASH_SIZE & 0xFFFEu);
    auto opcode = GetOpCode(static_cast<int16_t>(address * -1));
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = ctx.cpu.PC;
    auto expectedNewAddress = static_cast<uint16_t>(ctx.cpu.PC - address);

    auto cycles = subject.Execute(opcode, ctx);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 3u);
    ASSERT_EQ(ctx.cpu.PC, expectedNewAddress);
    ASSERT_EQ(ctx.cpu.SP, originalSP - sizeof(ctx.cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}
