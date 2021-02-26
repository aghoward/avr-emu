#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/in.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class INInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        INInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x0F) | ((src << 5) & 0x0600));
            auto dstValue = static_cast<uint16_t>((dst << 4) & 0x01F0);
            return static_cast<uint16_t>(OpCode::IN) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 64);
            auto dst = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        INInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(INInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(INInstructionTests, Matches_GivenINOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::IN);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(INInstructionTests, Execute_CopiesIOValueToRegister)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.GPIO[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = 0x0u;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], ctx.cpu.GPIO[src]);
}
