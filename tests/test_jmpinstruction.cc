#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/jmp.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class JMPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        JMPInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(src & 0x07u);
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::JMP) | srcValue | dstValue;
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

    public:
        JMPInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(JMPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(JMPInstructionTests, Matches_GivenJMPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::JMP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(JMPInstructionTests, Execute_LongJumpsToOperand)
{
    auto opcode = static_cast<uint16_t>(OpCode::JMP);
    auto address = static_cast<uint16_t>(
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_FLASH_SIZE) & 0xFFFEu);
    SetAddressOperand(address);

    auto cycles = subject.Execute(opcode, ctx);

    ASSERT_EQ(cycles, 3u);
    ASSERT_EQ(ctx.cpu.PC, address);
}
