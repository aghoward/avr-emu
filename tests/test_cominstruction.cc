#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/com.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class COMInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        COMInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::COM) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        COMInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(COMInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(COMInstructionTests, Matches_GivenCOMOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::COM);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(COMInstructionTests, Execute_GivenValue_SetsRegisterToComplimentValue)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) & 0xFFu;
    uint8_t expectedValue = ctx.cpu.R[dst] ^ 0xFFu;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}

TEST_F(COMInstructionTests, Execute_GivenSignBitNotSet_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) & 0x7Fu;
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(COMInstructionTests, Execute_GivenSignBitSet_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = (static_cast<uint8_t>(rand()) & 0xFFu) | 0x80u;
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(COMInstructionTests, Execute_GivenAllBitsSet_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0xFFu;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.C);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.N);
}
