#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/brbc.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class BRBCInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        BRBCInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(OpCode opcode, uint8_t src, int16_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>(((dst & 0x3F) | ((0x8000 & dst) >> 9)) << 3u);
            return static_cast<uint16_t>(opcode) | srcValue | dstValue;
        }

    public:
        BRBCInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(BRBCInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(BRBCInstructionTests, Matches_GivenBRBCOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BRBC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BRBCInstructionTests, Matches_GivenBRBSOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BRBS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenCarryFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 0u, 0x10);
    ctx.cpu.SREG.C = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenCarryFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 0u, 0x10);
    ctx.cpu.SREG.C = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenCarryFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 0u, -0x10);
    ctx.cpu.SREG.C = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, 0x10);
    ctx.cpu.SREG.Z = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, 0x10);
    ctx.cpu.SREG.Z = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, -0x10);
    ctx.cpu.SREG.Z = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, 0x10);
    ctx.cpu.SREG.N = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, 0x10);
    ctx.cpu.SREG.N = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, -0x10);
    ctx.cpu.SREG.N = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, 0x10);
    ctx.cpu.SREG.V = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, 0x10);
    ctx.cpu.SREG.V = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, -0x10);
    ctx.cpu.SREG.V = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, 0x10);
    ctx.cpu.SREG.S = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, 0x10);
    ctx.cpu.SREG.S = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, -0x10);
    ctx.cpu.SREG.S = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, 0x10);
    ctx.cpu.SREG.H = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, 0x10);
    ctx.cpu.SREG.H = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, -0x10);
    ctx.cpu.SREG.H = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, 0x10);
    ctx.cpu.SREG.T = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, 0x10);
    ctx.cpu.SREG.T = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, -0x10);
    ctx.cpu.SREG.T = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, 0x10);
    ctx.cpu.SREG.I = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, 0x10);
    ctx.cpu.SREG.I = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, -0x10);
    ctx.cpu.SREG.I = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, 0x10);
    ctx.cpu.SREG.C = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, 0x10);
    ctx.cpu.SREG.C = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, -0x10);
    ctx.cpu.SREG.C = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, 0x10);
    ctx.cpu.SREG.Z = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, 0x10);
    ctx.cpu.SREG.Z = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, -0x10);
    ctx.cpu.SREG.Z = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, 0x10);
    ctx.cpu.SREG.N = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, 0x10);
    ctx.cpu.SREG.N = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, -0x10);
    ctx.cpu.SREG.N = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, 0x10);
    ctx.cpu.SREG.V = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, 0x10);
    ctx.cpu.SREG.V = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, -0x10);
    ctx.cpu.SREG.V = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, 0x10);
    ctx.cpu.SREG.S = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, 0x10);
    ctx.cpu.SREG.S = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, -0x10);
    ctx.cpu.SREG.S = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, 0x10);
    ctx.cpu.SREG.H = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, 0x10);
    ctx.cpu.SREG.H = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, -0x10);
    ctx.cpu.SREG.H = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, 0x10);
    ctx.cpu.SREG.T = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, 0x10);
    ctx.cpu.SREG.T = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, -0x10);
    ctx.cpu.SREG.T = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, 0x10);
    ctx.cpu.SREG.I = false;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(ctx.cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, 0x10);
    ctx.cpu.SREG.I = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, -0x10);
    ctx.cpu.SREG.I = true;
    ctx.cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(ctx.cpu.PC, 0x32u);
}
