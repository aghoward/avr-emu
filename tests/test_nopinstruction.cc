#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/nop.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class NOPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        NOPInstruction subject;
        ExecutionContext ctx;

    public:
        NOPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(NOPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(NOPInstructionTests, Matches_GivenNOPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::NOP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(NOPInstructionTests, Execute_ConsumesOneCycle)
{
    auto opcode = static_cast<uint16_t>(OpCode::NOP);

    auto cycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(cycles, 1u);
}
