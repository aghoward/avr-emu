#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/eor.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class EORInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        EORInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 5) & 0x0200));
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x01F0);
            return static_cast<uint16_t>(OpCode::EOR) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 31);
            auto dst = static_cast<uint8_t>(src + 1);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        EORInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(EORInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(EORInstructionTests, Matches_GivenEOROpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::EOR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(EORInstructionTests, Execute_GivenSameValues_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = ctx.cpu.R[src];
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_EQ(ctx.cpu.R[dst], 0x0u);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.S);
}

TEST_F(EORInstructionTests, Execute_GivenNotSameValues_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = ctx.cpu.R[src] + 0x1u;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_GivenOneNegativeOperand_SetsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu;
    ctx.cpu.R[dst] = (static_cast<uint8_t>(rand()) % 0x7Fu) | 0x80u;
    ctx.cpu.SREG.N = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_TRUE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_GivenNoNegativeOperands_ClearsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu;
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu;
    ctx.cpu.SREG.N = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_PerformsExclusiveOrOperation)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.cpu.R[src] ^ ctx.cpu.R[dst];


    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}
