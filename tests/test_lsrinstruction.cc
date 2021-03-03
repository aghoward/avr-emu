#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/lsr.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LSRInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LSRInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LSR) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        LSRInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LSRInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LSRInstructionTests, Matches_GivenLSROpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LSR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LSRInstructionTests, Execute_GivenOne_ReturnsZero)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x01u;
    auto expectedValue = static_cast<uint8_t>(0x0u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}

TEST_F(LSRInstructionTests, Execute_GivenOne_SetsCarryOverflowZeroAndSignFlags)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x01u;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.Z);
}

TEST_F(LSRInstructionTests, Execute_GivenRandomValue_ShiftsValueRight)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand());
    auto expectedValue = static_cast<uint8_t>(ctx.cpu.R[dst] >> 1u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}
