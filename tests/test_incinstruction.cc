#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/inc.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class INCInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        INCInstruction subject;
        SRAM memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::INC) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        INCInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(INCInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(INCInstructionTests, Matches_GivenINCOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::INC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(INCInstructionTests, Execute_GivenValue_IncrementsValue)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand());
    auto expected = cpu.R[dst] + 1u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expected);
}

TEST_F(INCInstructionTests, Execute_Given255_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(255u);
    cpu.SREG.Z = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(INCInstructionTests, Execute_GivenNot255_ClearsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand() % 0xFF);
    cpu.SREG.Z = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(INCInstructionTests, Execute_GivenLargestPositive_SetsOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(0x7fu);
    cpu.SREG.V = false;
    cpu.SREG.S = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.N);
}

TEST_F(INCInstructionTests, Execute_GivenResultIsNegative_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(0x82u);
    cpu.SREG.N = false;
    cpu.SREG.S = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.S);
}

TEST_F(INCInstructionTests, Execute_GivenResultIsPositive_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand() % 0x7E);
    cpu.SREG.N = true;
    cpu.SREG.S = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.S);
}
