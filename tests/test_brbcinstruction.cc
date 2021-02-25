#include "core/cpu.h"
#include "core/memory.h"
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
        SRAM memory;
        CPU cpu;

        uint16_t GetOpCode(OpCode opcode, uint8_t src, int16_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>(((dst & 0x3F) | ((0x8000 & dst) >> 9)) << 3u);
            return static_cast<uint16_t>(opcode) | srcValue | dstValue;
        }

    public:
        BRBCInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
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
    cpu.SREG.C = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenCarryFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 0u, 0x10);
    cpu.SREG.C = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenCarryFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 0u, -0x10);
    cpu.SREG.C = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, 0x10);
    cpu.SREG.Z = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, 0x10);
    cpu.SREG.Z = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenZeroFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 1u, -0x10);
    cpu.SREG.Z = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, 0x10);
    cpu.SREG.N = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, 0x10);
    cpu.SREG.N = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenNegativeFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 2u, -0x10);
    cpu.SREG.N = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, 0x10);
    cpu.SREG.V = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, 0x10);
    cpu.SREG.V = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenOverflowFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 3u, -0x10);
    cpu.SREG.V = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, 0x10);
    cpu.SREG.S = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, 0x10);
    cpu.SREG.S = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenSignFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 4u, -0x10);
    cpu.SREG.S = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, 0x10);
    cpu.SREG.H = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, 0x10);
    cpu.SREG.H = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenHalfCarryFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 5u, -0x10);
    cpu.SREG.H = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, 0x10);
    cpu.SREG.T = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, 0x10);
    cpu.SREG.T = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenBitCopyFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 6u, -0x10);
    cpu.SREG.T = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, 0x10);
    cpu.SREG.I = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagNotSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, 0x10);
    cpu.SREG.I = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBC_GivenInterruptFlagNotSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBC, 7u, -0x10);
    cpu.SREG.I = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, 0x10);
    cpu.SREG.C = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, 0x10);
    cpu.SREG.C = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenCarryFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 0u, -0x10);
    cpu.SREG.C = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, 0x10);
    cpu.SREG.Z = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, 0x10);
    cpu.SREG.Z = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenZeroFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 1u, -0x10);
    cpu.SREG.Z = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, 0x10);
    cpu.SREG.N = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, 0x10);
    cpu.SREG.N = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenNegativeFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 2u, -0x10);
    cpu.SREG.N = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, 0x10);
    cpu.SREG.V = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, 0x10);
    cpu.SREG.V = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenOverflowFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 3u, -0x10);
    cpu.SREG.V = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, 0x10);
    cpu.SREG.S = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, 0x10);
    cpu.SREG.S = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenSignFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 4u, -0x10);
    cpu.SREG.S = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, 0x10);
    cpu.SREG.H = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, 0x10);
    cpu.SREG.H = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenHalfCarryFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 5u, -0x10);
    cpu.SREG.H = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, 0x10);
    cpu.SREG.T = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, 0x10);
    cpu.SREG.T = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenBitCopyFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 6u, -0x10);
    cpu.SREG.T = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagNotSet_DoesNotBranch)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, 0x10);
    cpu.SREG.I = false;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 1u);
    ASSERT_EQ(cpu.PC, 0x42u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagSet_BranchesForward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, 0x10);
    cpu.SREG.I = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x52u);
}

TEST_F(BRBCInstructionTests, ExecuteBRBS_GivenInterruptFlagSet_BranchesBackward)
{
    auto opcode = GetOpCode(OpCode::BRBS, 7u, -0x10);
    cpu.SREG.I = true;
    cpu.PC = 0x42;

    const auto consumedCycles = subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(consumedCycles, 2u);
    ASSERT_EQ(cpu.PC, 0x32u);
}
