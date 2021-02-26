#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/andi.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class ANDIInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        ANDIInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 4) & 0x0F00));
            auto dstValue = static_cast<uint16_t>((dst - 16u) << 4u);
            return static_cast<uint16_t>(OpCode::ANDI) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters(uint8_t src)
        {
            auto dst = static_cast<uint8_t>((rand() % 16) + 16);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            return GetRegisters(static_cast<uint8_t>(rand() % 256));
        }

    public:
        ANDIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ANDIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ANDIInstructionTests, Matches_GivenANDOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ANDI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ANDIInstructionTests, Execute_GivenSmallValues_DoesNotSetNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand() % 0x7f);
    auto expectedResult = src & ctx.cpu.R[dst];

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedResult);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_EQ(ctx.cpu.SREG.Z, expectedResult == 0u);
}

TEST_F(ANDIInstructionTests, Execute_GivenOppositeValues_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[dst] = src ^ 0xFFu;
    ctx.cpu.SREG.Z = 1;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], 0u);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(ANDIInstructionTests, Execute_ClearsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand() % 0x7f);
    ctx.cpu.SREG.V = 1;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(ANDIInstructionTests, Execute_GivenNegativeResult_SetsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters(static_cast<uint8_t>((rand() % 0x7f) | 0x80));
    ctx.cpu.R[dst] = static_cast<uint8_t>((rand() % 0x7f) | 0x80);
    ctx.cpu.SREG.N = 0;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
}
