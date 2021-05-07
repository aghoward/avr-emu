#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/call.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class CALLInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        CALLInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::CALL) | srcValue | dstValue;
        }

        void SetAddressOperand(uint16_t address)
        {
            auto pc = static_cast<uint16_t>(
                    static_cast<uint16_t>(rand()) %
                    (static_cast<uint16_t>(AVR_EMU_FLASH_SIZE) - 2u));
            ctx.cpu.PC = pc;
            auto shiftedAddress = static_cast<uint16_t>(address >> 1u);
            for (auto i = static_cast<uint16_t>(0u); i < sizeof(shiftedAddress); i++)
                ctx.progMem[pc++] = static_cast<uint8_t>((shiftedAddress >> (8u * i)) & 0xFFu);
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

        uint16_t ReadReturnAddress()
        {
            uint16_t address = 0u;
            uint16_t base_address = ctx.cpu.SP + 1u;
            for (uint16_t i = 0; i < sizeof(ctx.cpu.PC); i++)
                address |= static_cast<uint16_t>(ctx.ram[base_address + i] << ((sizeof(ctx.cpu.PC) - i - 1u) * 8u));
            return address;
        }

    public:
        CALLInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(CALLInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(CALLInstructionTests, Matches_GivenCALLOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CALL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CALLInstructionTests, Execute_LongJumpsToOperand)
{
    auto opcode = static_cast<uint16_t>(OpCode::CALL);
    auto address = static_cast<uint16_t>(
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_FLASH_SIZE) & 0xFFFEu);
    SetAddressOperand(address);
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = ctx.cpu.PC + static_cast<uint16_t>(sizeof(ctx.cpu.PC));

    auto cycles = subject.Execute(opcode, ctx);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 4u);
    ASSERT_EQ(ctx.cpu.PC, address);
    ASSERT_EQ(ctx.cpu.SP, originalSP - sizeof(ctx.cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}
