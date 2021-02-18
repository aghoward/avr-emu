#include "cpu.h"
#include "instructions/bclr.h"
#include "instructions/opcodes.h"
#include "memory.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class BCLRInstructionTests : public ::testing::Test
{

    protected:
        BCLRInstruction subject;
        CPU cpu;
        Memory memory;

        uint16_t GetOpCode(uint8_t src) const
        {
            auto srcValue = static_cast<uint16_t>((src << 4) & 0x0070);
            return static_cast<uint16_t>(OpCode::BCLR) | srcValue;
        }

        void SetAllFlags()
        {
            cpu.SREG.C = 
                cpu.SREG.Z = 
                cpu.SREG.N = 
                cpu.SREG.V = 
                cpu.SREG.S = 
                cpu.SREG.H = 
                cpu.SREG.T = 
                cpu.SREG.I =
                1;
        }

    public:
        BCLRInstructionTests() :
            subject(), cpu(), memory()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(BCLRInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(BCLRInstructionTests, Matches_GivenBCLROpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BCLR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BCLRInstructionTests, Execute_GivenZero_ClearsCarryFlag)
{
    auto opcode = GetOpCode(0u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenOne_ClearsZeroFlag)
{
    auto opcode = GetOpCode(1u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_FALSE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenTwo_ClearsNegativeFlag)
{
    auto opcode = GetOpCode(2u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenThree_ClearsOverflowFlag)
{
    auto opcode = GetOpCode(3u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenFour_ClearsSignFlag)
{
    auto opcode = GetOpCode(4u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_FALSE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenFive_ClearsHalfCarryFlag)
{
    auto opcode = GetOpCode(5u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_FALSE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenSix_ClearsBitCopyFlag)
{
    auto opcode = GetOpCode(6u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_FALSE(cpu.SREG.T);
    ASSERT_TRUE(cpu.SREG.I);
}

TEST_F(BCLRInstructionTests, Execute_GivenSeven_ClearsInterruptFlag)
{
    auto opcode = GetOpCode(7u);
    SetAllFlags();

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_TRUE(cpu.SREG.H);
    ASSERT_TRUE(cpu.SREG.T);
    ASSERT_FALSE(cpu.SREG.I);
}
