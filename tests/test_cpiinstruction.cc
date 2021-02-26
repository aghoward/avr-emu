#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/cpi.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class CPIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        CPIInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(OpCode opcode, uint8_t k, uint8_t dst) const
        {
            auto kValue = static_cast<uint16_t>(((k & 0xF0u) << 8u) | (k & 0x0Fu));
            auto dstValue = static_cast<uint16_t>(((dst - 16u) & 0x0Fu) << 4u);
            return static_cast<uint16_t>(opcode) | kValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters(uint8_t k)
        {
            auto dst = static_cast<uint8_t>((static_cast<uint8_t>(rand()) % 16u) + 16u);
            auto compiledOpcode = GetOpCode(OpCode::CPI, k, dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        CPIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(CPIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(CPIInstructionTests, Matches_GivenCPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CPI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CPIInstructionTests, Execute_GivenBorrowFromBit3_SetsHalfCarryFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    ctx.cpu.SREG.H = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.H);
}

TEST_F(CPIInstructionTests, Execute_GivenNoBorrowFromBit3_ClearsHalfCarryFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    ctx.cpu.SREG.H = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.H);
}

TEST_F(CPIInstructionTests, Execute_GivenResultWrapsPositive_SetsOverflowFlag)
{
    auto k = static_cast<uint8_t>(-3u);
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(CPIInstructionTests, Execute_GivenResultWrapsNegative_SetsOverflowFlag)
{
    uint8_t k = 0x7u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x82u);
    ctx.cpu.SREG.V = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(CPIInstructionTests, Execute_GivenEqualValues_SetsZeroFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0xFFu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = k;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(CPIInstructionTests, Execute_GivenPositiveSourceGreaterThanPositiveDestination_SetsCarryFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0x7Fu;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(k - 1u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(CPIInstructionTests, Execute_GivenAbsoluteSourceGreaterThanAbsoluteDestination_SetsCarryFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0xFFu | 0x80u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>(k - 1u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(CPIInstructionTests, Execute_GivenNegativeSourceAbsoluteGreaterThanPositiveDestination_SetsCarryFlag)
{
    uint8_t k = static_cast<uint8_t>(rand()) % 0x7Eu | 0x80u;
    auto [opcode, dst] = GetRegisters(k);
    ctx.cpu.R[dst] = static_cast<uint8_t>((k ^ 0x80u) - 1u);
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}
