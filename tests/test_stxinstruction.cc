#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/stx.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class STXInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        STXInstruction subject;
        ExecutionContext ctx;

        std::tuple<uint16_t, uint16_t, uint16_t> GetRegisters(bool pre_decrement, bool post_increment)
        {
            auto offset = static_cast<uint16_t>(rand() % (ctx.ram.size() / 2));
            auto srcIndex = static_cast<uint16_t>(rand() % 26);
            auto compiledOpcode = static_cast<uint16_t>(OpCode::STX)
                | ((srcIndex & 0x1Fu) << 4u)
                | (pre_decrement ? 0x02u : 0x00u)
                | (post_increment ? 0x01u : 0x00u);
            return std::make_tuple(compiledOpcode, offset, srcIndex);
        }


    public:
        STXInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
            ctx.cpu.PC = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
        }
};

TEST_F(STXInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(STXInstructionTests, Matches_GivenSTOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::STX);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STXInstructionTests, Matches_GivenSTOpCodeWithPreDecrement_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::STX) | 0x2u;
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STXInstructionTests, Matches_GivenSTOpCodeWithPostIncrement_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::STX) | 0x1u;
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STXInstructionTests, Execute_StoresContentOfRegisterIntoMemory)
{
    auto [opcode, offset, src] = GetRegisters(false, false);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.X = offset;
    ctx.ram[offset] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[offset], 0xAB);
}

TEST_F(STXInstructionTests, Execute_GivenPreDecrement_StoresContentsAtPreviousAddress)
{
    auto [opcode, offset, src] = GetRegisters(true, false);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.X = offset;
    ctx.ram[offset] = 0xFF;
    auto expectedOffset = offset - 1;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[expectedOffset], 0xAB);
    ASSERT_EQ(*ctx.cpu.X, expectedOffset);
}

TEST_F(STXInstructionTests, Execute_GivenPostIncrement_StoresContentsAtOriginalAddress)
{
    auto [opcode, offset, src] = GetRegisters(false, true);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.X = offset;
    ctx.ram[offset] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[offset], 0xAB);
    ASSERT_EQ(*ctx.cpu.X, offset + 1);
}
