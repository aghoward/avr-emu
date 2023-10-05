#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/stz.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class STZInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        STZInstruction subject;
        ExecutionContext ctx;

        std::tuple<uint16_t, uint16_t, uint16_t, uint8_t> GetRegisters(bool pre_decrement, bool post_increment, bool has_displacement)
        {
            auto offset = static_cast<uint16_t>(rand() % (ctx.ram.size() / 2));
            auto srcIndex = static_cast<uint16_t>(rand() % 26);
            auto displacement = static_cast<uint16_t>((has_displacement) ? 
                    rand() % 64u
                    : 0u);
            auto displacementSplit = ((displacement & 0x0020u) << 8u) | ((displacement & 0x0018u) << 7) | (displacement & 0x7u);
            auto compiledOpcode = static_cast<uint16_t>(OpCode::STZ)
                | ((srcIndex & 0x1Fu) << 4u)
                | ((pre_decrement) ? 0x1002u : 0x00u)
                | ((post_increment) ? 0x1001u : 0x00u)
                | displacementSplit;
            return std::make_tuple(compiledOpcode, offset, srcIndex, displacement);
        }


    public:
        STZInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
            ctx.cpu.PC = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
        }
};

TEST_F(STZInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(STZInstructionTests, Matches_GivenSTOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::STZ);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STZInstructionTests, Matches_GivenSTOpCodeWithPreDecrement_ReturnsTrue)
{
    auto [opcode, offset, src, displacement] = GetRegisters(true, false, false);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STZInstructionTests, Matches_GivenSTOpCodeWithPostIncrement_ReturnsTrue)
{
    auto [opcode, offset, src, displacement] = GetRegisters(false, true, false);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STZInstructionTests, Matches_GivenSTOpCodeWithDisplacement_ReturnsTrue)
{
    auto [opcode, offset, src, displacement] = GetRegisters(false, false, true);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STZInstructionTests, Execute_StoresContentOfRegisterIntoMemory)
{
    auto [opcode, offset, src, displacement] = GetRegisters(false, false, false);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.Z = offset;
    ctx.ram[offset] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[offset], 0xAB);
}

TEST_F(STZInstructionTests, Execute_GivenDisplacement_StoresContentOfRegisterIntoMemoryWithDisplacement)
{
    auto [opcode, offset, src, displacement] = GetRegisters(false, false, true);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.Z = offset;
    ctx.ram[offset + displacement] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[offset + displacement], 0xAB);
}

TEST_F(STZInstructionTests, Execute_GivenPreDecrement_StoresContentsAtPreviousAddress)
{
    auto [opcode, offset, src, displacement] = GetRegisters(true, false, false);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.Z = offset;
    auto expectedOffset = offset - 1;
    ctx.ram[expectedOffset] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[expectedOffset], 0xAB);
    ASSERT_EQ(*ctx.cpu.Z, expectedOffset);
}

TEST_F(STZInstructionTests, Execute_GivenPostIncrement_StoresContentsAtOriginalAddress)
{
    auto [opcode, offset, src, displacement] = GetRegisters(false, true, false);
    ctx.cpu.R[src] = 0xAB;
    ctx.cpu.Z = offset;
    ctx.ram[offset] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(displacement, 0u);
    ASSERT_EQ(ctx.ram[offset], 0xAB);
    ASSERT_EQ(*ctx.cpu.Z, offset + 1);
}
