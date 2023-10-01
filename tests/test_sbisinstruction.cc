#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sbis.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class SBISInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SBISInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t bit, uint8_t io) const
        {
            auto srcValue = static_cast<uint16_t>(bit & 0x07u);
            auto dstValue = static_cast<uint16_t>((io & 0x1Fu) << 3u);
            return static_cast<uint16_t>(OpCode::SBIS) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto bit = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 0x08u);
            auto io = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(bit, io);
            return std::make_tuple(std::move(compiledOpcode), bit, io);
        }

        void WriteOpCodeToPC(OpCode opcode)
        {
            auto op = static_cast<uint16_t>(opcode);
            for (uint16_t i = 0u; i < sizeof(op); i++)
                ctx.progMem[ctx.cpu.PC + i] = static_cast<uint8_t>(op >> (i * 8u));
        }

    public:
        SBISInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
            ctx.cpu.PC = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
        }
};

TEST_F(SBISInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SBISInstructionTests, Matches_GivenSBISOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SBIS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SBISInstructionTests, Execute_GivenBitClear_DoesNotSkipNextInstruction)
{
    auto [opcode, bit, io] = GetRegisters();
    ctx.cpu.GPIO[io] = (0xFFu ^ (0x1u << bit));
    auto expectedPC = ctx.cpu.PC;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, expectedPC);
}

TEST_F(SBISInstructionTests, Execute_GivenBitSetAndSingleWordOpcode_SkipsOneWord)
{
    auto [opcode, bit, io] = GetRegisters();
    ctx.cpu.GPIO[io] |= (0x1u << bit);
    WriteOpCodeToPC(OpCode::INC);
    auto expectedPC = ctx.cpu.PC + 2;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, expectedPC);
}

TEST_F(SBISInstructionTests, Execute_GivenBitSetAndDoubleWordOpcode_SkipsTwoWords)
{
    auto [opcode, bit, io] = GetRegisters();
    ctx.cpu.GPIO[io] |= (0x1u << bit);
    WriteOpCodeToPC(OpCode::CALL);
    auto expectedPC = ctx.cpu.PC + 4;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, expectedPC);
}
