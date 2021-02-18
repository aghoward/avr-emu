#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/asr.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class ASRInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        ASRInstruction subject;
        Memory memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x01F0);
            return static_cast<uint16_t>(OpCode::ASR) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        ASRInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ASRInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ASRInstructionTests, Matches_GivenASROpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ASR);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ASRInstructionTests, Execute_GivenPositiveValue_ShiftsRightOneBit)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand() % 0x7f);
    auto expectedResult = cpu.R[dst] >> 1;
    auto hadCarry = (cpu.R[dst] & 0x1) == 0x1u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedResult);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_EQ(cpu.SREG.C, hadCarry);
    ASSERT_EQ(cpu.SREG.S, hadCarry);
    ASSERT_EQ(cpu.SREG.V, hadCarry);
    ASSERT_EQ(cpu.SREG.Z, expectedResult == 0u);
}

TEST_F(ASRInstructionTests, Execute_GivenValue1_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(0x1);
    auto expectedResult = static_cast<uint8_t>(0x0u);
    auto hadCarry = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedResult);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_EQ(cpu.SREG.C, hadCarry);
    ASSERT_EQ(cpu.SREG.S, hadCarry);
    ASSERT_EQ(cpu.SREG.V, hadCarry);
    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(ASRInstructionTests, Execute_GivenNegativeValue_RetainsNegativeBit)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>((static_cast<uint8_t>(rand()) % 0x7fu) | 0x80u);
    auto expectedResult = static_cast<uint8_t>((cpu.R[dst] >> 1) | 0x80u);
    auto hadCarry = (cpu.R[dst] & 0x1) == 0x1u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedResult);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_EQ(cpu.SREG.C, hadCarry);
    ASSERT_EQ(cpu.SREG.S, hadCarry);
    ASSERT_EQ(cpu.SREG.V, !hadCarry);
    ASSERT_EQ(cpu.SREG.Z, expectedResult == 0u);
}

