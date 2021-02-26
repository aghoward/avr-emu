#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/las.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LASInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LASInstruction subject;
        SRAM memory;
        CPU cpu;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LAS) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 32u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        void InitializeZRegister()
        {
            cpu.Z = static_cast<uint16_t>(rand()) % AVR_EMU_RAM_SIZE;
            memory[*cpu.Z] = 0x0u;
        }

    public:
        LASInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LASInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LASInstructionTests, Matches_GivenLASOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LAS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LASInstructionTests, Execute_SetsBitsFromDestinationInMemoryStore)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = 0xFFu;
    InitializeZRegister();
    memory[*cpu.Z] = 0x00u;

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], 0x00u);
    ASSERT_EQ(memory[*cpu.Z], 0xFFu);
}

TEST_F(LASInstructionTests, Execute_StoresContentsOfMemoryIntoRegister)
{
    auto [opcode, dst] = GetRegisters();
    cpu.R[dst] = static_cast<uint8_t>(rand());
    InitializeZRegister();
    auto expectedRegister = static_cast<uint8_t>(rand());
    memory[*cpu.Z] = expectedRegister;
    auto expectedMemory = static_cast<uint8_t>(expectedRegister | cpu.R[dst]);

    subject.Execute(opcode, cpu, memory);

    ASSERT_EQ(cpu.R[dst], expectedRegister);
    ASSERT_EQ(memory[*cpu.Z], expectedMemory);
}
