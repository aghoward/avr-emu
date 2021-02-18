#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/cbi.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class CBIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        CBIInstruction subject;
        Memory memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 3u);
            return static_cast<uint16_t>(OpCode::CBI) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 0x08u);
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        CBIInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(CBIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(CBIInstructionTests, Matches_GivenCBIOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CBI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CBIInstructionTests, Execute_GivenBitClear_IsNoop)
{
    auto [opcode, src, dst] = GetRegisters();
    uint8_t expectedValue = 0xFFu ^ static_cast<uint8_t>(0x1u << src);
    cpu.GPIO[dst] = expectedValue;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.GPIO[dst], expectedValue);
}

TEST_F(CBIInstructionTests, Execute_GivenBitSet_ClearsBit)
{
    auto [opcode, src, dst] = GetRegisters();
    uint8_t expectedValue = 0xFFu ^ static_cast<uint8_t>(0x1u << src);
    cpu.GPIO[dst] = static_cast<uint8_t>(0xFFu);

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.GPIO[dst], expectedValue);
}
