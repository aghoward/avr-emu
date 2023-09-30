#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/ror.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class RORInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        RORInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(int16_t dst)
        {
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::ROR) |
                (dst << 4));
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        RORInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(RORInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(RORInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ROR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(RORInstructionTests, Execute_GivenPositiveInteger_DividesByTwo)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand() & 0x7fu);
    auto expected = static_cast<uint8_t>(ctx.cpu.R[dst] / 2u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(RORInstructionTests, Execute_GivenNegativeInteger_DividesByTwo)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<int8_t>(rand() & 0x7f) * -1;
    auto expected = static_cast<int8_t>(ctx.cpu.R[dst] / 2);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(RORInstructionTests, Execute_GivenLowestBitSet_SetsCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(RORInstructionTests, Execute_GivenLowestBitNotSet_ClearsCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0xFEu);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(RORInstructionTests, Execute_GivenCarryBitClearBeforeOperation_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = true;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(RORInstructionTests, Execute_GivenCarryBitSetBeforeOperation_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x74u);
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(RORInstructionTests, Execute_GivenNegativeFlagSetByOperation_SetsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x74u);
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(RORInstructionTests, Execute_GivenCarryFlagSetByOperation_SetsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(RORInstructionTests, Execute_GivenCarryAndNegativeFlagsSetByOperation_ClearsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01u);
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.V = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(RORInstructionTests, Execute_GivenNegativeFlagClearedAndOverflowSetByOperation_SetsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = true;
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(RORInstructionTests, Execute_GivenCarryAndNegativeFlagsSetByOperation_SetsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x81u);
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(RORInstructionTests, Execute_GivenOverlowAndNegativeFlagsSetByOperation_ClearsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x44u);
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(RORInstructionTests, Execute_GivenAllBitsClearedByOperation_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01u);
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(RORInstructionTests, Execute_GivenAnyBitsSetByOperation_ClearsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x80u);
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}
