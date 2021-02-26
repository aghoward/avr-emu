#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/adiw.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class ADIWInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        ADIWInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dstIndex) const
        {
            auto dst = static_cast<uint8_t>((dstIndex - 24u) / 2);
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 2) & 0x00C0));
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x0030);
            return static_cast<uint16_t>(OpCode::ADIW) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 63 + 1);
            auto dst = static_cast<uint8_t>((rand() % 4) * 2 + 24);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), std::move(src), std::move(dst));
        }

    public:
        ADIWInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ADIWInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ADIWInstructionTests, Matches_GivenADIWOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ADIW);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ADIWInstructionTests, Execute_GivenNoOverflow_CanAddTwoRegisters)
{
    auto [opcode, addend, dstIndex] = GetRegisters();
    ctx.cpu.R[dstIndex] = 0x42u;
    ctx.cpu.R[dstIndex+1] = 0x0u;
    ctx.cpu.SREG.C = 0;

    subject.Execute(opcode, ctx);

    auto result = static_cast<uint16_t>((ctx.cpu.R[dstIndex]) | (ctx.cpu.R[dstIndex+1] << 8));
    ASSERT_EQ(result, addend + 0x42u);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ADIWInstructionTests, Execute_GivenLowByteOverflow_CanAddTwoRegisters)
{
    auto [opcode, addend, dstIndex] = GetRegisters();
    ctx.cpu.R[dstIndex] = 0xFFu;
    ctx.cpu.R[dstIndex+1] = 0x0u;
    ctx.cpu.SREG.C = 0;

    subject.Execute(opcode, ctx);

    auto result = static_cast<uint16_t>((ctx.cpu.R[dstIndex]) | (ctx.cpu.R[dstIndex+1] << 8));
    ASSERT_EQ(result, addend + 0xFFu);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ADIWInstructionTests, Execute_GivenHighByteOverflow_SetsStatusRegister)
{
    auto [opcode, addend, dstIndex] = GetRegisters();
    ctx.cpu.R[dstIndex] = 0xffu;
    ctx.cpu.R[dstIndex+1] = 0x7fu;
    ctx.cpu.SREG.C = 0;

    subject.Execute(opcode, ctx);

    auto result = static_cast<uint16_t>((ctx.cpu.R[dstIndex]) | (ctx.cpu.R[dstIndex+1] << 8));
    ASSERT_EQ(result, addend + 0x7FFFu);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_TRUE(ctx.cpu.SREG.V);
    ASSERT_TRUE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ADIWInstructionTests, Execute_GivenCarry_SetsStatusRegister)
{
    auto [opcode, addend, dstIndex] = GetRegisters();
    ctx.cpu.R[dstIndex] = 0xffu;
    ctx.cpu.R[dstIndex+1] = 0xffu;
    ctx.cpu.SREG.C = 0;

    subject.Execute(opcode, ctx);

    auto result = static_cast<uint16_t>((ctx.cpu.R[dstIndex]) | (ctx.cpu.R[dstIndex+1] << 8));
    ASSERT_EQ(result, (addend + 0xFFFFu) & 0xFFFFu);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_EQ(ctx.cpu.SREG.Z, result == 0u);
    ASSERT_TRUE(ctx.cpu.SREG.C);
}
