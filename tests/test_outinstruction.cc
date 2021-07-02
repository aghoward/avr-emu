#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/out.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>


using namespace avr;

class OUTInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        OUTInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x0F) | ((dst << 5) & 0x0600));
            auto srcValue = static_cast<uint16_t>((src << 4) & 0x01F0);
            return static_cast<uint16_t>(OpCode::OUT) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(rand() % 64);
            auto src = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        OUTInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(OUTInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(OUTInstructionTests, Matches_GivenOUTOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::OUT);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(OUTInstructionTests, Execute_CopiesRegisterValueToIORegister)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.GPIO[dst] = 0x0u;
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.GPIO[dst], ctx.cpu.R[src]);
}
