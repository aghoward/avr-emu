#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/ret.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class RETInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        RETInstruction subject;
        ExecutionContext ctx;

        void SetReturnAddress(uint16_t address)
        {
            for (auto i = static_cast<uint16_t>(0u); i < sizeof(address); i++)
                ctx.ram[ctx.cpu.SP--] = static_cast<uint8_t>((address >> (8u * i)) & 0xFFu);
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

        uint16_t GetExpectedReturnAddress()
        {
            auto address = static_cast<uint16_t>(
                static_cast<uint16_t>(rand()) % AVR_EMU_FLASH_SIZE);
            InitializeSP();
            SetReturnAddress(address);
            return address;
        }

    public:
        RETInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(RETInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(RETInstructionTests, Matches_GivenRETOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::RET);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(RETInstructionTests, Execute_SetsPCToReturnAddress)
{
    auto expectedAddress = GetExpectedReturnAddress();
    auto expectedSP = ctx.cpu.SP + sizeof(ctx.cpu.PC);

    auto cycles = subject.Execute(static_cast<uint16_t>(OpCode::RET), ctx);

    ASSERT_EQ(cycles, 4u);
    ASSERT_EQ(ctx.cpu.PC, expectedAddress);
    ASSERT_EQ(ctx.cpu.SP, expectedSP);
}
