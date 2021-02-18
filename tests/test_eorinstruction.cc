#include "core/cpu.h"
#include "core/memory.h"
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
        Memory memory;
        CPU cpu;

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
            clock(), subject(clock), memory(), cpu(memory)
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
    cpu.R[src] = static_cast<uint8_t>(rand());
    cpu.R[dst] = cpu.R[src];
    cpu.SREG.Z = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
    ASSERT_EQ(cpu.R[dst], 0x0u);
    ASSERT_FALSE(cpu.SREG.V);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.S);
}

TEST_F(EORInstructionTests, Execute_GivenNotSameValues_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.R[src] = static_cast<uint8_t>(rand());
    cpu.R[dst] = cpu.R[src] + 0x1u;
    cpu.SREG.Z = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.Z);
    ASSERT_FALSE(cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_GivenOneNegativeOperand_SetsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu;
    cpu.R[dst] = (static_cast<uint8_t>(rand()) % 0x7Fu) | 0x80u;
    cpu.SREG.N = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.S);
    ASSERT_FALSE(cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_GivenNoNegativeOperands_ClearsNegativeFlag)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu;
    cpu.SREG.N = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.S);
    ASSERT_FALSE(cpu.SREG.V);
}

TEST_F(EORInstructionTests, Execute_PerformsExclusiveOrOperation)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.R[src] = static_cast<uint8_t>(rand());
    cpu.R[dst] = static_cast<uint8_t>(rand());
    auto expectedValue = cpu.R[src] ^ cpu.R[dst];


    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedValue);
}
