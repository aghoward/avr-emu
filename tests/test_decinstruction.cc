#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/dec.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class DECInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        DECInstruction subject;
        SRAM memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::DEC) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

    public:
        DECInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(DECInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(DECInstructionTests, Matches_GivenDECOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::DEC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(DECInstructionTests, Execute_GivenValue_DecrementsValue)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand());
    auto expected = cpu.R[dst] - 1u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expected);
}

TEST_F(DECInstructionTests, Execute_GivenOne_SetsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(1u);

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(DECInstructionTests, Execute_GivenNotOne_ClearsZeroFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = (static_cast<uint8_t>(rand()) % 0xFE) + 1u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(DECInstructionTests, Execute_GivenSmallestNegative_SetsOverflowFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(0x80u);

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.S);
}

TEST_F(DECInstructionTests, Execute_GivenResultIsNegative_SetsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(0x00u);

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_TRUE(cpu.SREG.S);
}

TEST_F(DECInstructionTests, Execute_GivenResultIsPositive_ClearsNegativeFlag)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = (static_cast<uint8_t>(rand()) % 0x7E) + 0x01u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.S);
}
