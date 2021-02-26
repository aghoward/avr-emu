#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/ld.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LDInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LDInstruction subject;
        SRAM memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LD) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        void InitializeXRegister()
        {
            cpu.X = static_cast<uint16_t>(rand()) % AVR_EMU_RAM_SIZE;
            memory[*cpu.X] = 0x0u;
        }

    public:
        LDInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LDInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LDInstructionTests, Matches_GivenLDOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LD);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LDInstructionTests, Execute_GivenNoIncrementOrDecrement_LoadsMemoryAddressToRegister)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = 0x00u;
    InitializeXRegister();
    memory[*cpu.X] = static_cast<uint8_t>(rand());
    auto expectedValue = memory[*cpu.X];
    auto expectedX = *cpu.X;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedValue);
    ASSERT_EQ(*cpu.X, expectedX);
}

TEST_F(LDInstructionTests, Execute_GivenPostIncrement_LoadsMemoryAddressToRegisterAndIncrementsX)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = 0x00u;
    InitializeXRegister();
    memory[*cpu.X] = static_cast<uint8_t>(rand());
    auto expectedValue = memory[*cpu.X];
    auto expectedX = *cpu.X + 1u;

    subject.Execute(static_cast<uint16_t>(opcode | 0x1), cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedValue);
    ASSERT_EQ(*cpu.X, expectedX);
}

TEST_F(LDInstructionTests, Execute_GivenPreDecrement_LoadsPreviousMemoryAddressToRegisterAndDecrementsX)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = 0x00u;
    InitializeXRegister();
    auto expectedX = static_cast<uint16_t>(*cpu.X - 1u);
    memory[expectedX] = static_cast<uint8_t>(rand());
    auto expectedValue = memory[expectedX];

    subject.Execute(static_cast<uint16_t>(opcode | 0x2), cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedValue);
    ASSERT_EQ(*cpu.X, expectedX);
}
