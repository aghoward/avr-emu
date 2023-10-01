#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sbc.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class SBCInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SBCInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint16_t src, uint16_t dst)
        {
            auto source = ((src & 0x10u) << 5) | ((src & 0x0Fu));
            auto destination = ((dst & 0x1Fu) << 4);
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::SBC) |
                source | destination);
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 16u);
            auto src = dst + 4u;
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        SBCInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SBCInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SBCInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SBC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SBCInstructionTests, Execute_GivenTwoPositiveIntegers_SubtractsIntegers)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(45u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(75u);
    auto expected = static_cast<uint8_t>(30u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expected);
}

TEST_F(SBCInstructionTests, Execute_GivenNegativeSource_AddsIntegers)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(-5);
    ctx.cpu.R[dst] = static_cast<int8_t>(39);
    auto expected = static_cast<int8_t>(44);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCInstructionTests, Execute_GivenNegativeDestination_SubtractsIntegers)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(5);
    ctx.cpu.R[dst] = static_cast<int8_t>(-39);
    auto expected = static_cast<int8_t>(-44);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCInstructionTests, Execute_GivenCarryFlagSet_SubtractsOneFromResult)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(3);
    ctx.cpu.R[dst] = static_cast<int8_t>(5);
    ctx.cpu.SREG.C = true;
    auto expected = static_cast<int8_t>(1);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(static_cast<int8_t>(ctx.cpu.R[dst]), expected);
}

TEST_F(SBCInstructionTests, Execute_GivenSourceLargerThanDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(96);
    ctx.cpu.R[dst] = static_cast<int8_t>(95);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCInstructionTests, Execute_GivenSourceEqualToDestinationAndCarrySet_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(96);
    ctx.cpu.R[dst] = static_cast<int8_t>(96);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCInstructionTests, Execute_GivenSourceAbsoluteValueLargerThanDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(-96);
    ctx.cpu.R[dst] = static_cast<int8_t>(95);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCInstructionTests, Execute_GivenSourceAbsoluteValueEqualToDestinationAndCarrySet_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<int8_t>(-96);
    ctx.cpu.R[dst] = static_cast<int8_t>(96);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBCInstructionTests, Execute_GivenResultIsPositive_ClearsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x3u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = true;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(SBCInstructionTests, Execute_GivenResultIsNegative_SetsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x35u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x34u);
    ctx.cpu.SREG.N = false;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(SBCInstructionTests, Execute_GivenPositiveResult_ClearsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x0Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(SBCInstructionTests, Execute_GivenNegativeResult_SetsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x8Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(SBCInstructionTests, Execute_GivenPositiveSourceNegativeDestinationAndPositiveResult_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x81u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(SBCInstructionTests, Execute_GivenPositiveDestinationNegativeSourceAndNegativeResult_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(-0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(SBCInstructionTests, Execute_GivenPositiveResult_ClearsSignFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x15u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x3Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(SBCInstructionTests, Execute_GivenNegativeResult_SetsSignFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x15u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x0Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBCInstructionTests, Execute_OverflowWithPositiveResult_SetsSignFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x81u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBCInstructionTests, Execute_OverflowWithNegativeResult_ClearsSignFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(-0x04u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(SBCInstructionTests, Execute_GivenNoBorrowFromDestBit4_ClearsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x02u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x05u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.H);
}

TEST_F(SBCInstructionTests, Execute_GivenBorrowFromDestBit4_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x08u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x10u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCInstructionTests, Execute_GivenSourceAndResultSetBit4_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x98u);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x07u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCInstructionTests, Execute_GivenResultSetAndDestinationNotSetBit4_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x00u);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(SBCInstructionTests, Execute_GivenResultZeroAndZeroFlagSet_ZeroFlagIsUnchanged)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(SBCInstructionTests, Execute_GivenResultZeroAndZeroFlagClear_ZeroFlagIsUnchanged)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(SBCInstructionTests, Execute_GivenResultNonZero_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(0x8Fu);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x9Fu);
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}
