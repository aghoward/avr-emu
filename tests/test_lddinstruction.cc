#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/ldd.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LDDInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LDDInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LDD) | dstValue;
        }

        uint16_t GetOpCodeWithDisplacement(uint8_t dst, uint8_t displacement) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            auto displacementValue = static_cast<uint16_t>(
                ((displacement & 0x20u) << 8u) |
                ((displacement & 0x18u) << 7u) |
                (displacement & 0x7u));
            return static_cast<uint16_t>(0x8008u) | dstValue | displacementValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 26u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegistersWithDisplacement()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 26u);
            auto displacement = static_cast<uint8_t>(static_cast<uint8_t>(rand()) & 0x3fu);
            auto compiledOpcode = GetOpCodeWithDisplacement(dst, displacement);
            return std::make_tuple(std::move(compiledOpcode), dst, displacement);
        }

        void InitializeYRegister()
        {
            ctx.cpu.Y = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + ctx.cpu.SRAM_BEG;
            ctx.ram[*ctx.cpu.Y] = 0x0u;
        }

    public:
        LDDInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LDDInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LDDInstructionTests, Matches_GivenLDDOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LDD);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LDDInstructionTests, Execute_GivenNoIncrementOrDecrement_LoadsMemoryAddressToRegister)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;
    InitializeYRegister();
    ctx.ram[*ctx.cpu.Y] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[*ctx.cpu.Y];
    auto expectedY = *ctx.cpu.Y;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Y, expectedY);
}

TEST_F(LDDInstructionTests, Execute_GivenPostIncrement_LoadsMemoryAddressToRegisterAndIncrementsY)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;
    InitializeYRegister();
    ctx.ram[*ctx.cpu.Y] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[*ctx.cpu.Y];
    auto expectedY = *ctx.cpu.Y + 1u;

    subject.Execute(static_cast<uint16_t>(opcode | 0x1001), ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Y, expectedY);
}

TEST_F(LDDInstructionTests, Execute_GivenPreDecrement_LoadsPreviousMemoryAddressToRegisterAndDecrementsY)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x00u;
    InitializeYRegister();
    auto expectedY = static_cast<uint16_t>(*ctx.cpu.Y - 1u);
    ctx.ram[expectedY] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[expectedY];

    subject.Execute(static_cast<uint16_t>(opcode | 0x1002), ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Y, expectedY);
}

TEST_F(LDDInstructionTests, Execute_GivenDisplacement_LoadsDisplacedMemoryAddressToRegister)
{
    auto [opcode, dst, displacement] = GetRegistersWithDisplacement();
    ctx.cpu.R[dst] = 0x00u;
    InitializeYRegister();
    auto expectedY = *ctx.cpu.Y;
    auto expectedLocation = static_cast<uint16_t>(expectedY + displacement);
    ctx.ram[expectedLocation] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.ram[expectedLocation];

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Y, expectedY);
}
