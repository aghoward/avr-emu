#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/muls.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class MULSInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        MULSInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src - 16) & 0x0F);
            auto dstValue = static_cast<uint16_t>(((dst - 16) << 4) & 0x0F0);
            return static_cast<uint16_t>(OpCode::MULS) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>((rand() % 15) + 16);
            auto dst = static_cast<uint8_t>(src + 1);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        int16_t ReadRegisterPair() const
        {
            auto value = static_cast<int16_t>(0);
            for (auto i = 0u; i < sizeof(value); i++)
                value |= static_cast<int16_t>((ctx.cpu.R[i] << (i * 8)) & 0xFFFFu);
            return value;
        }

    public:
        MULSInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(MULSInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(MULSInstructionTests, Matches_GivenMULSOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::MULS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(MULSInstructionTests, Execute_GivenPositiveValues_MultipliesPositiveValues)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = 0x10u;
    ctx.cpu.R[dst] = 0x10u;
    auto expectedValue = 0x100;
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(), expectedValue);
    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(MULSInstructionTests, Execute_GivenOneNegativeValue_ReturnsNegativeValue)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = 0xFFu;
    ctx.cpu.R[dst] = 0x02u;
    auto expectedValue = -2;
    ctx.cpu.SREG.C = false;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(), expectedValue);
    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(MULSInstructionTests, Execute_GivenTwoNegativeValue_ReturnsPositiveValue)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = 0xf0u;
    ctx.cpu.R[dst] = 0xf0u;
    auto expectedValue = 0x100u;
    ctx.cpu.SREG.C = true;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(), expectedValue);
    ASSERT_FALSE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(MULSInstructionTests, Execute_GivenResultIsZero_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = 0x0u;
    auto expectedValue = 0x0;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(), expectedValue);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(MULSInstructionTests, Execute_GivenHighestBitOfResultSet_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = 0xf0u;
    ctx.cpu.R[dst] = 0x02u;
    auto expectedValue = -32;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(), expectedValue);
    ASSERT_TRUE(ctx.cpu.SREG.C);
}
