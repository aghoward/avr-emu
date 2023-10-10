#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/xch.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class XCHInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        XCHInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint16_t src)
        {
            return static_cast<uint16_t>(
                ((src & 0x1Fu) << 4) |
                static_cast<uint16_t>(OpCode::XCH));
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint16_t>(rand() % 30);
            auto compiledOpcode = GetOpCode(src);
            ctx.cpu.Z = static_cast<uint16_t>(rand() % (ctx.ram.size() / 2));
            return std::make_tuple(std::move(compiledOpcode), src);
        }

    public:
        XCHInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(XCHInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(XCHInstructionTests, Matches_GivenCorrectOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::XCH);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(XCHInstructionTests, Execute_SwapsRamContentsWithSourceRegisterContents)
{
    auto [opcode, dst] = GetRegisters();
    ctx.ram[*ctx.cpu.Z] = 0x009Au;
    ctx.cpu.R[dst] = static_cast<uint8_t>(0xA5);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], 0x9Au);
    ASSERT_EQ(ctx.ram[*ctx.cpu.Z], 0xA5);
}
