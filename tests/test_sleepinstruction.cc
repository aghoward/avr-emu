#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sleep.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class SLEEPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SLEEPInstruction subject;
        ExecutionContext ctx;

    public:
        SLEEPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SLEEPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SLEEPInstructionTests, Matches_GivenSLEEPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SLEEP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SLEEPInstructionTests, Execute_PutsCPUToSleep)
{
    auto opcode = static_cast<uint16_t>(OpCode::SLEEP);
    ASSERT_FALSE(ctx.cpu.is_sleeping);

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.is_sleeping);
}
