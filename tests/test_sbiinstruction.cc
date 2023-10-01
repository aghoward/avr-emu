#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sbi.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class SBIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SBIInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t bit, uint8_t io) const
        {
            auto srcValue = static_cast<uint16_t>(bit & 0x07u);
            auto dstValue = static_cast<uint16_t>((io & 0x1Fu) << 3u);
            return static_cast<uint16_t>(OpCode::SBI) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto bit = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 0x08u);
            auto io = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(bit, io);
            return std::make_tuple(std::move(compiledOpcode), bit, io);
        }

    public:
        SBIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(SBIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SBIInstructionTests, Matches_GivenSBIOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SBI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SBIInstructionTests, Execute_GivenBitClear_SetsBit)
{
    auto [opcode, bit, io] = GetRegisters();
    uint8_t expectedValue = static_cast<uint8_t>(0x1u << bit);
    ctx.cpu.GPIO[io] = expectedValue;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.GPIO[io], expectedValue);
}

TEST_F(SBIInstructionTests, Execute_GivenBitSet_IsNoop)
{
    auto [opcode, bit, io] = GetRegisters();
    uint8_t expectedValue = static_cast<uint8_t>(0xFFu);
    ctx.cpu.GPIO[io] = static_cast<uint8_t>(0xFFu);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.GPIO[io], expectedValue);
}
