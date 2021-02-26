#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/ldi.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LDIInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LDIInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst, uint8_t k) const
        {
            auto dstValue = static_cast<uint16_t>(((dst - 16u) & 0x0Fu) << 4u);
            auto kValue = static_cast<uint16_t>(((k & 0xF0u) << 4u) | (k & 0x0Fu));
            return static_cast<uint16_t>(OpCode::LDI) | dstValue | kValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>((rand() % 16) + 16);
            auto k = static_cast<uint8_t>(rand());
            auto compiledOpcode = GetOpCode(dst, k);
            return std::make_tuple(std::move(compiledOpcode), dst, k);
        }

    public:
        LDIInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LDIInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LDIInstructionTests, Matches_GivenLDIOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LDI);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LDIInstructionTests, Execute_LoadsRegisterWithImmediateValue)
{
    auto [opcode, dst, k] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], k);
}
