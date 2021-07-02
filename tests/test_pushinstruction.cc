#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/push.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class PUSHInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        PUSHInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src) const
        {
            auto srcValue = static_cast<uint16_t>((src & 0x1F) << 4);
            return static_cast<uint16_t>(OpCode::PUSH) | srcValue;
        }

        std::tuple<uint16_t, uint8_t, uint16_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(src);
            return std::make_tuple(std::move(compiledOpcode), src, InitializeSP());
        }

        uint16_t InitializeSP()
        {
            auto usableRamSize = static_cast<uint16_t>(AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG);
            uint16_t sp = static_cast<uint16_t>(rand() % usableRamSize) + 2u + ctx.cpu.SRAM_BEG;
            ctx.cpu.SP = sp;
            return sp;
        }

    public:
        PUSHInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(PUSHInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(PUSHInstructionTests, Matches_GivenPUSHOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::PUSH);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(PUSHInstructionTests, Execute_WritesValueAtCurrentSP)
{
    auto [opcode, src, sp] = GetRegisters();
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.ram[sp] = 0u;
    ctx.cpu.R[src] = expectedValue;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.ram[sp], expectedValue);
    ASSERT_EQ(ctx.cpu.SP, sp-1);
}
