#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/add.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class ADDInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        ADDInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(OpCode opcode, uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 5) & 0x0200));
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x01F0);
            return static_cast<uint16_t>(opcode) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters(OpCode opcode)
        {
            auto src = static_cast<uint8_t>(rand() % 31);
            auto dst = static_cast<uint8_t>(src + 1);
            auto compiledOpcode = GetOpCode(opcode, src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        void Execute_GivenNoCarry_CanAddTwoRegisters(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 2;
            ctx.cpu.R[dst] = 3;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 5);
            ASSERT_FALSE(ctx.cpu.SREG.H);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_FALSE(ctx.cpu.SREG.V);
            ASSERT_FALSE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.Z);
            ASSERT_FALSE(ctx.cpu.SREG.C);
        }

        void Execute_GivenLowNibbleOverflow_SetsHalfCarry(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 0x0C;
            ctx.cpu.R[dst] = 0x05;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 0x11);
            ASSERT_TRUE(ctx.cpu.SREG.H);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_FALSE(ctx.cpu.SREG.V);
            ASSERT_FALSE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.Z);
            ASSERT_FALSE(ctx.cpu.SREG.C);
        }

        void Execute_GivenHighNibbleOverflow_SetsCarry(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 0xF0;
            ctx.cpu.R[dst] = 0x50;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 0x40);
            ASSERT_FALSE(ctx.cpu.SREG.H);
            ASSERT_FALSE(ctx.cpu.SREG.V);
            ASSERT_FALSE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_FALSE(ctx.cpu.SREG.Z);
            ASSERT_TRUE(ctx.cpu.SREG.C);
        }

        void Execute_GivenTwosComplimentOverflow_SetsOverflowFlag(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 0x7C;
            ctx.cpu.R[dst] = 0x05;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 0x81);
            ASSERT_TRUE(ctx.cpu.SREG.H);
            ASSERT_TRUE(ctx.cpu.SREG.V);
            ASSERT_TRUE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_FALSE(ctx.cpu.SREG.Z);
            ASSERT_FALSE(ctx.cpu.SREG.C);
        }

        void Execute_GivenResultIsZero_SetsZeroFlag(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 0x00;
            ctx.cpu.R[dst] = 0x00;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 0x0);
            ASSERT_FALSE(ctx.cpu.SREG.H);
            ASSERT_FALSE(ctx.cpu.SREG.V);
            ASSERT_FALSE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_TRUE(ctx.cpu.SREG.Z);
            ASSERT_FALSE(ctx.cpu.SREG.C);
        }

        void Execute_GivenAnyRegisterSet_AddsRegisters(OpCode addCode)
        {
            auto [opcode, src, dst] = GetRegisters(addCode);
            ctx.cpu.R[src] = 0x02;
            ctx.cpu.R[dst] = 0x02;
            ctx.cpu.SREG.C = 0;

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.R[dst], 0x04);
            ASSERT_FALSE(ctx.cpu.SREG.H);
            ASSERT_FALSE(ctx.cpu.SREG.V);
            ASSERT_FALSE(ctx.cpu.SREG.N);
            ASSERT_FALSE(ctx.cpu.SREG.S);
            ASSERT_FALSE(ctx.cpu.SREG.Z);
            ASSERT_FALSE(ctx.cpu.SREG.C);
        }
    public:
        ADDInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ADDInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ADDInstructionTests, Matches_GivenADCOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ADC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ADDInstructionTests, Matches_GivenADDOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ADD);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenNoCarry_CanAddTwoRegisters)
{
    Execute_GivenNoCarry_CanAddTwoRegisters(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenNoCarry_CanAddTwoRegisters)
{
    Execute_GivenNoCarry_CanAddTwoRegisters(OpCode::ADD);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenCarry_AddsCarryToAnswer)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::ADC);
    ctx.cpu.R[src] = 2;
    ctx.cpu.R[dst] = 3;
    ctx.cpu.SREG.C = 1;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], 6);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenCarry_DoesNotAddCarry)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::ADD);
    ctx.cpu.R[src] = 2;
    ctx.cpu.R[dst] = 3;
    ctx.cpu.SREG.C = 1;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], 5);
    ASSERT_FALSE(ctx.cpu.SREG.H);
    ASSERT_FALSE(ctx.cpu.SREG.S);
    ASSERT_FALSE(ctx.cpu.SREG.V);
    ASSERT_FALSE(ctx.cpu.SREG.N);
    ASSERT_FALSE(ctx.cpu.SREG.Z);
    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenLowNibbleOverflow_SetsHalfCarry)
{
    Execute_GivenLowNibbleOverflow_SetsHalfCarry(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenLowNibbleOverflow_SetsHalfCarry)
{
    Execute_GivenLowNibbleOverflow_SetsHalfCarry(OpCode::ADD);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenHighNibbleOverflow_SetsCarry)
{
    Execute_GivenHighNibbleOverflow_SetsCarry(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenHighNibbleOverflow_SetsCarry)
{
    Execute_GivenHighNibbleOverflow_SetsCarry(OpCode::ADD);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenTwosComplimentOverflow_SetsOverflowFlag)
{
    Execute_GivenTwosComplimentOverflow_SetsOverflowFlag(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenTwosComplimentOverflow_SetsOverflowFlag)
{
    Execute_GivenTwosComplimentOverflow_SetsOverflowFlag(OpCode::ADD);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenResultIsZero_SetsZeroFlag)
{
    Execute_GivenResultIsZero_SetsZeroFlag(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenResultIsZero_SetsZeroFlag)
{
    Execute_GivenResultIsZero_SetsZeroFlag(OpCode::ADD);
}

TEST_F(ADDInstructionTests, ExecuteADC_GivenAnyRegisterSet_AddsRegisters)
{
    Execute_GivenAnyRegisterSet_AddsRegisters(OpCode::ADC);
}

TEST_F(ADDInstructionTests, ExecuteADD_GivenAnyRegisterSet_AddsRegisters)
{
    Execute_GivenAnyRegisterSet_AddsRegisters(OpCode::ADD);
}
