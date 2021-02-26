#include "core/executioncontext.h"
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
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LD) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 26u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        void InitializeXRegister()
        {
            ctx.cpu.X = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + ctx.cpu.SRAM_BEG;
            ctx.ram[*ctx.cpu.X] = 0x0u;
        }

    public:
        LDInstructionTests() :
            clock(), subject(clock), ctx()
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
    ctx.cpu.R[dst] = 0x00u;
    InitializeXRegister();
    ctx.ram[*ctx.cpu.X] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[*ctx.cpu.X];
    auto expectedX = *ctx.cpu.X;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.X, expectedX);
}

TEST_F(LDInstructionTests, Execute_GivenPostIncrement_LoadsMemoryAddressToRegisterAndIncrementsX)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;
    InitializeXRegister();
    ctx.ram[*ctx.cpu.X] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[*ctx.cpu.X];
    auto expectedX = *ctx.cpu.X + 1u;

    subject.Execute(static_cast<uint16_t>(opcode | 0x1), ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.X, expectedX);
}

TEST_F(LDInstructionTests, Execute_GivenPreDecrement_LoadsPreviousMemoryAddressToRegisterAndDecrementsX)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;
    InitializeXRegister();
    auto expectedX = static_cast<uint16_t>(*ctx.cpu.X - 1u);
    ctx.ram[expectedX] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[expectedX];

    subject.Execute(static_cast<uint16_t>(opcode | 0x2), ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.X, expectedX);
}
