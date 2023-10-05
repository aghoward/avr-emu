#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/swap.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class SWAPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SWAPInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint16_t src)
        {
            return static_cast<uint16_t>(
                ((src & 0x1Fu) << 4) |
                static_cast<uint16_t>(OpCode::SWAP));
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint16_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(src);
            return std::make_tuple(std::move(compiledOpcode), src);
        }

    public:
        SWAPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SWAPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SWAPInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SWAP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SWAPInstructionTests, Execute_SwapsNibblesInRegister)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0xA5);
    auto expected = static_cast<uint8_t>(0x5A);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}
