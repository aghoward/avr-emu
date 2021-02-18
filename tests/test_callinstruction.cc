#include "core/cpu.h"
#include "core/memory.h"
#include "instructions/call.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>

using namespace avr;

class CALLInstructionTests : public ::testing::Test
{

    protected:
        CALLInstruction subject;
        Memory memory;
        CPU cpu;

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
                    (static_cast<uint16_t>(AVR_EMU_RAM_SIZE) - 2u)
                    + cpu.SRAM_BEG);
            cpu.PC = pc;
            auto shiftedAddress = static_cast<uint16_t>(address >> 1u);
            for (auto i = static_cast<uint16_t>(0u); i < sizeof(shiftedAddress); i++)
                memory[pc++] = static_cast<uint8_t>((shiftedAddress >> (8u * i)) & 0xFFu);
        }

        uint16_t InitializeSP()
        {
            uint16_t sp = static_cast<uint16_t>(rand()) + 2u + cpu.SRAM_BEG;
            cpu.SP = sp;
            for (uint16_t i = 0u; i < sizeof(cpu.PC); i++)
                memory[i] = 0u;
            return sp;
        }

        uint16_t ReadReturnAddress()
        {
            uint16_t address = 0u;
            uint16_t base_address = cpu.SP + 1u;
            for (uint16_t i = 0; i < sizeof(cpu.PC); i++)
                address |= static_cast<uint16_t>(memory[base_address + i] << ((sizeof(cpu.PC) - i - 1u) * 8u));
            return address;
        }

    public:
        CALLInstructionTests() :
            subject(), memory(), cpu(memory)
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
            static_cast<uint16_t>(rand()) % static_cast<uint16_t>(AVR_EMU_RAM_SIZE) & 0xFFFEu);
    SetAddressOperand(address);
    auto originalSP = InitializeSP();
    auto expectedReturnAddress = cpu.PC + static_cast<uint16_t>(sizeof(cpu.PC));

    auto cycles = subject.Execute(opcode, cpu, memory);
    auto actualReturn = ReadReturnAddress();

    ASSERT_EQ(cycles, 4u);
    ASSERT_EQ(cpu.PC, address);
    ASSERT_EQ(cpu.SP, originalSP - sizeof(cpu.PC));
    ASSERT_EQ(actualReturn, expectedReturnAddress);
}
