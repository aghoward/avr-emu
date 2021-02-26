#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/bst.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class BSTInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        BSTInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::BST) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 0x08u);
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        BSTInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(BSTInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(BSTInstructionTests, Matches_GivenBSTOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::BST);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(BSTInstructionTests, Execute_GivenBitClear_ClearsBit)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0xFFu ^ static_cast<uint8_t>(0x1u << src);
    ctx.cpu.SREG.T = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.T);
}

TEST_F(BSTInstructionTests, Execute_GivenBitSet_SetsBit)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(0x1u << src);
    ctx.cpu.SREG.T = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.T);
}
