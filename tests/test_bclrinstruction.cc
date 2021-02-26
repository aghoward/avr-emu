#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/bclr.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class BCLRInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        BCLRInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(OpCode opcode, uint8_t src) const
        {
            auto srcValue = static_cast<uint16_t>((src << 4) & 0x0070);
            return static_cast<uint16_t>(opcode) | srcValue;
        }

        void SetAllFlags(bool value)
        {
            ctx.cpu.SREG.C =
                ctx.cpu.SREG.Z =
                ctx.cpu.SREG.N =
                ctx.cpu.SREG.V =
                ctx.cpu.SREG.S =
                ctx.cpu.SREG.H =
                ctx.cpu.SREG.T =
                ctx.cpu.SREG.I =
                value;
        }

        void SetAllFlags()
        {
            SetAllFlags(true);
        }

        void ClearAllFlags()
        {
            SetAllFlags(false);
        }

    public:
        BCLRInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(BCLRInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(BCLRInstructionTests, Matches_GivenBCLROpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BCLR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BCLRInstructionTests, Matches_GivenBSETOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BSET);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenZero_ClearsCarryFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 0u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenOne_ClearsZeroFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 1u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenTwo_ClearsNegativeFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 2u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenThree_ClearsOverflowFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 3u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenFour_ClearsSignFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 4u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenFive_ClearsHalfCarryFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 5u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenSix_ClearsBitCopyFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 6u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBCLR_GivenSeven_ClearsInterruptFlag)
{
    auto opcode = GetOpCode(OpCode::BCLR, 7u);
    SetAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenZero_SetsCarryFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 0u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenOne_SetsZeroFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 1u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenTwo_SetsNegativeFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 2u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenThree_SetsOverflowFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 3u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenFour_SetsSignFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 4u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenFive_SetsHalfCarryFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 5u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenSix_SetsBitCopyFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 6u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_TRUE(ctx.cpu.SREG.T);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, ExecuteBSET_GivenSeven_SetsInterruptFlag)
{
    auto opcode = GetOpCode(OpCode::BSET, 7u);
    ClearAllFlags();

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.T);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

