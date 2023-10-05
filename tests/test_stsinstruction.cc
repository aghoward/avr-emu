#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/sts.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class STSInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        STSInstruction subject;
        ExecutionContext ctx;

        std::tuple<uint16_t, uint8_t, uint16_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 32);
            auto address = static_cast<uint16_t>(rand() % ctx.ram.size());
            auto compiledOpcode = static_cast<uint16_t>(OpCode::STS) | ((src & 0x1fu) << 4);
            ctx.cpu.PC = 0x100u;
            auto pcWriter = IndirectRegister(&ctx.progMem[ctx.cpu.PC]);
            pcWriter = address;
            return std::make_tuple(compiledOpcode, src, address);
        }


    public:
        STSInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
            ctx.cpu.PC = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
        }
};

TEST_F(STSInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(STSInstructionTests, Matches_GivenSTSOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::STS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(STSInstructionTests, Execute_StoresSourceValueIntoMemoryAtAddress)
{
    auto [opcode, src, address] = GetRegisters();
    ctx.cpu.R[src] = 0xAB;
    ctx.ram[address] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[address], 0xAB);
}

TEST_F(STSInstructionTests, Execute_IncremetsPCByTwo)
{
    auto [opcode, src, address] = GetRegisters();
    ctx.cpu.R[src] = 0xAB;
    ctx.ram[address] = 0xFF;
    auto originalPC = ctx.cpu.PC;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, originalPC + 2);
}
