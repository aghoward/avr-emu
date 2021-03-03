#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/mov.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class MOVInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        MOVInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 5) & 0x0200));
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x01F0);
            return static_cast<uint16_t>(OpCode::MOV) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 31);
            auto dst = static_cast<uint8_t>(src + 1);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        MOVInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(MOVInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(MOVInstructionTests, Matches_GivenMOVOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::MOV);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(MOVInstructionTests, Execute_CopiesValueFromSourceToDestination)
{
    auto [opcode, src, dst] = GetRegisters();
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.cpu.R[src] = expectedValue;
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand());

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[src], expectedValue);
    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}
