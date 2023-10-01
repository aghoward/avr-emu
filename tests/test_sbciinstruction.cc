#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sbci.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class SBCIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SBCIInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint16_t dst, uint8_t k)
        {
            auto split_k = ((k & 0xF0u) << 4u) | (k & 0x0Fu);
            auto destination = ((dst & 0x0Fu) << 4);
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::SBCI) |
                split_k | destination);
        }

        std::tuple<uint16_t, uint8_t> GetRegisters(uint8_t k)
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 16u) | 0x10u;
            auto compiledOpcode = GetOpCode(dst, k);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        SBCIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SBCIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SBCIInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SBCI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SBCIInstructionTests, Execute_GivenTwoPositiveIntegers_SubtractsIntegers)
{
    auto k = 45u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(75u);
    auto expected = static_cast<uint8_t>(30u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(SBCIInstructionTests, Execute_GivenNegativeSource_AddsIntegers)
{
    auto k = -5u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(39);
    auto expected = static_cast<int8_t>(44);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCIInstructionTests, Execute_GivenNegativeDestination_SubtractsIntegers)
{
    auto k = 5u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(-39);
    auto expected = static_cast<int8_t>(-44);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCIInstructionTests, Execute_GivenCarryFlagSet_SubtractsOneFromResult)
{
    auto k = 3u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(5);
    ctx.cpu.SREG.C = true;
    auto expected = static_cast<int8_t>(1);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCIInstructionTests, Execute_GivenSourceLargerThanDestination_SetsCarryFlag)
{
    auto k = 96u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(95);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCIInstructionTests, Execute_GivenSourceEqualToDestinationAndCarrySet_SetsCarryFlag)
{
    auto k = 96u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(96);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCIInstructionTests, Execute_GivenSourceAbsoluteValueLargerThanDestination_SetsCarryFlag)
{
    auto k = -96u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(95);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCIInstructionTests, Execute_GivenSourceAbsoluteValueEqualToDestinationAndCarrySet_SetsCarryFlag)
{
    auto k = -96u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<int8_t>(96);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultIsPositive_ClearsNegativeFlag)
{
    auto k = 0x3u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = true;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultIsNegative_SetsNegativeFlag)
{
    auto k = 0x35u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(SBCIInstructionTests, Execute_GivenPositiveResult_ClearsNegativeFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x0Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(SBCIInstructionTests, Execute_GivenNegativeResult_SetsNegativeFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x8Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(SBCIInstructionTests, Execute_GivenPositiveSourceNegativeDestinationAndPositiveResult_SetsOverflowFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x81u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(SBCIInstructionTests, Execute_GivenPositiveDestinationNegativeSourceAndNegativeResult_SetsOverflowFlag)
{
    auto k = -0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(SBCIInstructionTests, Execute_GivenPositiveResult_ClearsSignFlag)
{
    auto k = 0x15u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x3Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(SBCIInstructionTests, Execute_GivenNegativeResult_SetsSignFlag)
{
    auto k = 0x15u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x0Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBCIInstructionTests, Execute_OverflowWithPositiveResult_SetsSignFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x81u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBCIInstructionTests, Execute_OverflowWithNegativeResult_ClearsSignFlag)
{
    auto k = -0x04u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(SBCIInstructionTests, Execute_GivenNoBorrowFromDestBit4_ClearsHalfCarryFlag)
{
    auto k = 0x02u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x05u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.H);
}

TEST_F(SBCIInstructionTests, Execute_GivenBorrowFromDestBit4_SetsHalfCarryFlag)
{
    auto k = 0x08u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x10u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCIInstructionTests, Execute_GivenSourceAndResultSetBit4_SetsHalfCarryFlag)
{
    auto k = 0x98u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x07u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultSetAndDestinationNotSetBit4_SetsHalfCarryFlag)
{
    auto k = 0x9Fu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x00u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultZeroAndZeroFlagSet_ZeroFlagIsUnchanged)
{
    auto k = 0x9Fu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultZeroAndZeroFlagClear_ZeroFlagIsUnchanged)
{
    auto k = 0x9Fu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(SBCIInstructionTests, Execute_GivenResultNonZero_ClearsZeroFlag)
{
    auto k = 0x8Fu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}
