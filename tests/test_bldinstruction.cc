#include "core/cpu.h"
#include "core/memory.h"
#include "instructions/bld.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class BLDInstructionTests : public ::testing::Test
{

    protected:
        BLDInstruction subject;
        Memory memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::BLD) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 0x08u);
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        BLDInstructionTests() :
            subject(), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(BLDInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(BLDInstructionTests, Matches_GivenBLDOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BLD);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BLDInstructionTests, Execute_GivenFlagSet_SetsBitInDestination)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.SREG.T = true;
    cpu.R[dst] &= 0xFFu ^ static_cast<uint8_t>(0x1u << src);

    subject.Execute(opcode, cpu, memory);

    ASSERT_NE(cpu.R[dst] & (0x1u << src), 0x0u);
}

TEST_F(BLDInstructionTests, Execute_GivenFlagClear_ClearsBitInDestination)
{
    auto [opcode, src, dst] = GetRegisters();
    cpu.SREG.T = false;
    cpu.R[dst] |= static_cast<uint8_t>(0x1u << src);

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst] & (0x1u << src), 0x0u);
}
