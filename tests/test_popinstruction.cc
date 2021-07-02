#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/pop.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class POPInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        POPInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1F) << 4);
            return static_cast<uint16_t>(OpCode::POP) | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint16_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst, InitializeSP());
        }

        uint16_t InitializeSP()
        {
            auto usableRamSize = static_cast<uint16_t>(AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG);
            uint16_t sp = static_cast<uint16_t>(rand() % usableRamSize) + 2u + ctx.cpu.SRAM_BEG;
            ctx.cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(ctx.cpu.PC); i++)
                ctx.ram[i] = 0u;
            return sp;
        }

    public:
        POPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(POPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(POPInstructionTests, Matches_GivenPOPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::POP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(POPInstructionTests, Execute_ReadsValueAfterCurrentSP)
{
    auto [opcode, dst, sp] = GetRegisters();
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.ram[sp+1] = expectedValue;
    ctx.cpu.R[dst] = 0u;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(ctx.cpu.SP, sp+1);
}
