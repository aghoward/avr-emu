#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sbiw.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class SBIWInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SBIWInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t index, uint8_t k)
        {
            auto split_k = ((k & 0x30u) << 2u) | (k & 0x0Fu);
            auto destination = ((index & 0x03u) << 4);
            return static_cast<uint16_t>(
                static_cast<uint16_t>(OpCode::SBIW) |
                split_k | destination);
        }

        std::tuple<uint16_t, uint8_t> GetRegisters(uint8_t k)
        {
            auto index = static_cast<uint8_t>(rand() % 4u);
            auto compiledOpcode = GetOpCode(index, k);
            auto lowRegister = static_cast<uint8_t>((index * 2) + 24);
            return std::make_tuple(std::move(compiledOpcode), lowRegister);
        }

        void SetValue(uint8_t dst, uint16_t value)
        {
            ctx.cpu.R[dst] = static_cast<uint8_t>(value & 0xFFu);
            ctx.cpu.R[dst+1] = static_cast<uint8_t>((value & 0xFF00u) >> 8u);
        }

        uint16_t GetResult(uint8_t dst) const
        {
            return static_cast<uint16_t>(ctx.cpu.R[dst+1] << 8) | static_cast<uint16_t>(ctx.cpu.R[dst]);
        }

    public:
        SBIWInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SBIWInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SBIWInstructionTests, Matches_GivenROLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SBIW);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SBIWInstructionTests, Execute_GivenTwoPositiveIntegers_SubtractsIntegers)
{
    auto k = 45u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x1433u);
    auto expected = static_cast<uint16_t>(5126u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(GetResult(dst), expected);
}

TEST_F(SBIWInstructionTests, Execute_GivenNegativeDestination_SubtractsIntegers)
{
    auto k = 5u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, -12);
    auto expected = static_cast<uint16_t>(-17);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(GetResult(dst), expected);
}

TEST_F(SBIWInstructionTests, Execute_GivenImmediateLargerThanDestination_SetsCarryFlag)
{
    auto k = 34u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 32u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(SBIWInstructionTests, Execute_GivenImmediateEqualToDestination_ClearsCarryFlag)
{
    auto k = 34u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 34u);
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(SBIWInstructionTests, Execute_GivenResultIsPositive_ClearsNegativeFlag)
{
    auto k = 0x3u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x34u);
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
}

TEST_F(SBIWInstructionTests, Execute_GivenResultIsNegative_SetsNegativeFlag)
{
    auto k = 0x5u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 4u);
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}

TEST_F(SBIWInstructionTests, Execute_GivenPositiveResultAndNegativeDestination_SetsOverflowFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x8002u);
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
}

TEST_F(SBIWInstructionTests, Execute_GivenPositiveResult_ClearsSignFlag)
{
    auto k = 0x15u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x003Fu);
    ctx.cpu.SREG.S = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(SBIWInstructionTests, Execute_GivenNegativeResult_SetsSignFlag)
{
    auto k = 0x15u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x000Fu);
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBIWInstructionTests, Execute_GivenOverflowWithPositiveResult_SetsSignFlag)
{
    auto k = 0x04u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x8001u);
    ctx.cpu.SREG.S = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.S);
}

TEST_F(SBIWInstructionTests, Execute_GivenNonZeroResult_ClearsZeroFlag)
{
    auto k = 0x09u;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x0045u);
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(SBIWInstructionTests, Execute_GivenZeroResult_SetsZeroFlag)
{
    auto k = 0x0Fu;
    auto [opcode, dst] = GetRegisters(k);
    SetValue(dst, 0x000Fu);
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}
