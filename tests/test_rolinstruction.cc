#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/rol.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class ROLInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        ROLInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(int16_t dst)
        {
            auto dstMask = 0x01F0u;
            auto srcMask = 0x020Fu;
            auto destination = ((dstMask >> 4) & dst) << 4;
            auto source = (((srcMask >> 8) & dst) << 8) | ((srcMask & 0x0Fu) & dst);

            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::ROL) |
                destination | source);
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        ROLInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ROLInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ROLInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ROL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ROLInstructionTests, Execute_GivenPositiveInteger_MultiplesByTwo)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand() & 0x7fu);
    auto expected = static_cast<uint8_t>(ctx.cpu.R[dst] * 2u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(ROLInstructionTests, Execute_GivenNegativeInteger_MultiplesByTwo)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<int8_t>(rand() & 0x7f) * -1;
    auto expected = static_cast<int8_t>(ctx.cpu.R[dst] * 2);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestBitSet_SetsCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x80u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestBitNotSet_ClearsCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7fu);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestLowNibbleBitSetByRotation_SetsHalfCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x04u);
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestLowNibbleBitNotSetByRotation_ClearsHalfCarryFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0xFBu);
    ctx.cpu.SREG.H = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.H);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestBitClearAfterOperation_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(ROLInstructionTests, Execute_GivenHighestBitSetAfterOperation_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x74u);
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(ROLInstructionTests, Execute_GivenNegativeFlagSetByOperation_SetsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x74u);
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(ROLInstructionTests, Execute_GivenCarryFlagSetByOperation_SetsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x84u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(ROLInstructionTests, Execute_GivenCarryAndNegativeFlagsSetByOperation_ClearsTwosComplimentOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0xC4u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.V = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(ROLInstructionTests, Execute_GivenNegativeFlagClearedAndOverflowSetByOperation_SetsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x80);
    ctx.cpu.SREG.N = true;
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(ROLInstructionTests, Execute_GivenOverflowFlagSetByOperation_SetsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x84u);
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(ROLInstructionTests, Execute_GivenOverlowAndNegativeFlagsSetByOperation_ClearsSignFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x44u);
    ctx.cpu.SREG.V = false;
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(ROLInstructionTests, Execute_GivenAllBitsClearedByOperation_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x80u);
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(ROLInstructionTests, Execute_GivenAnyBitsSetByOperation_ClearsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x01u);
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}
