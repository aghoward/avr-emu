#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/neg.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class NEGInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        NEGInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::NEG) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        NEGInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(NEGInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(NEGInstructionTests, Matches_GivenNEGOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::NEG);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(NEGInstructionTests, Execute_GivenPositiveValue_ReturnsNegativeValue)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) & static_cast<uint8_t>(0x7Fu);
    auto expected = static_cast<uint8_t>(ctx.cpu.R[dst] * -1u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(NEGInstructionTests, Execute_GivenNegativeValue_ReturnsPositiveValue)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) | static_cast<uint8_t>(0x80u);
    auto expected = static_cast<uint8_t>(ctx.cpu.R[dst] * -1u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(NEGInstructionTests, Execute_GivenCarryFromBit3_SetsHalfCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) | 0x04u;
    ctx.cpu.SREG.H = 0;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(NEGInstructionTests, Execute_GivenNegativeZero_SetsOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x80u;
    ctx.cpu.SREG.V = 0;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(NEGInstructionTests, Execute_GivenNotNegativeZero_ClearsOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) & 0x7Fu;
    ctx.cpu.SREG.V = 1;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(NEGInstructionTests, Execute_GivenPositiveValue_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = (static_cast<uint8_t>(rand()) & 0x7Fu) | 0x1u;
    ctx.cpu.SREG.N = 0;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(NEGInstructionTests, Execute_GivenNegativeValue_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) | 0x82u;
    ctx.cpu.SREG.N = 1;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(NEGInstructionTests, Execute_GivenZero_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x0u;
    ctx.cpu.SREG.Z = 0;
    ctx.cpu.SREG.C = 1;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(NEGInstructionTests, Execute_GivenNonZero_ClearsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) | 0x1u;
    ctx.cpu.SREG.Z = 1;
    ctx.cpu.SREG.C = 0;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}
