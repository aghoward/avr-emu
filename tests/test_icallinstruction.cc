#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/icall.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class ICALLInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        ICALLInstruction subject;
        ExecutionContext ctx;

        uint16_t InitializeSP()
        {
            uint16_t sp = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + 2u + ctx.cpu.SRAM_BEG;
            ctx.cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(ctx.cpu.PC); i++)
                ctx.ram[sp - i] = 0u;
            return sp;
        }

        uint16_t ReadReturnAddress()
        {
            uint16_t address = 0u;
            uint16_t base_address = ctx.cpu.SP + 1u;
            for (uint16_t i = 0; i < sizeof(ctx.cpu.PC); i++)
                address |= static_cast<uint16_t>(ctx.ram[base_address + i] << ((sizeof(ctx.cpu.PC) - i - 1u) * 8u));
            return address;
        }

    public:
        ICALLInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ICALLInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(ICALLInstructionTests, Matches_GivenICALLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::ICALL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(ICALLInstructionTests, Execute_LongJumpsToOperand)
{
    auto opcode = static_cast<uint16_t>(OpCode::ICALL);
    auto address = static_cast<uint16_t>(
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_FLASH_SIZE) & 0xFFFEu);
    ctx.cpu.Z = address;
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = ctx.cpu.PC;

    auto cycles = subject.Execute(opcode, ctx);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 3u);
    ASSERT_EQ(ctx.cpu.PC, address);
    ASSERT_EQ(ctx.cpu.SP, originalSP - sizeof(ctx.cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}
