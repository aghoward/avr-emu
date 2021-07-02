#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/rjmp.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class RJMPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        RJMPInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(int16_t relativePosition)
        {
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::RJMP) |
                (relativePosition & 0x0FFF));
        }

    public:
        RJMPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(RJMPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(RJMPInstructionTests, Matches_GivenRJMPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::RJMP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(RJMPInstructionTests, Execute_GivenPositiveOffset_JumpsForward)
{
    auto address = static_cast<int16_t>(
            static_cast<int16_t>(rand() & 0x07FF) % AVR_EMU_FLASH_SIZE & 0xFFFEu);
    auto opcode = GetOpCode(address);
    auto expectedNewAddress = static_cast<uint16_t>(ctx.cpu.PC + address);

    auto cycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(cycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, expectedNewAddress);
}

TEST_F(RJMPInstructionTests, Execute_GivenNegativeOffset_JumpsBackward)
{
    auto address = static_cast<int16_t>(
            static_cast<int16_t>(
                static_cast<int16_t>(rand() & 0x07FF) % AVR_EMU_FLASH_SIZE & 0xFFFE) * -1);
    auto opcode = GetOpCode(address);
    auto expectedNewAddress = static_cast<uint16_t>(ctx.cpu.PC + address);

    auto cycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(cycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, expectedNewAddress);
}
