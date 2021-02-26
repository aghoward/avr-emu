#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/lpm.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LPMInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LPMInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint16_t opcode, uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return opcode | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters(uint16_t opcode)
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 26u);
            auto compiledOpcode = GetOpCode(opcode, dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        void InitializeZRegister()
        {
            ctx.cpu.Z = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + ctx.cpu.SRAM_BEG;
            ctx.progMem[*ctx.cpu.Z] = 0x0u;
        }

    public:
        LPMInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LPMInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LPMInstructionTests, Matches_GivenLPMOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LPM);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LPMInstructionTests, Matches_GivenLPMWithRegisterOpCode_ReturnsTrue)
{
    auto opcode = GetOpCode(static_cast<uint16_t>(0x9004u), static_cast<uint8_t>(rand() % 32));
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LPMInstructionTests, Matches_GivenLPMWithIncrement_ReturnsTrue)
{
    auto opcode = GetOpCode(static_cast<uint16_t>(0x9005u), static_cast<uint8_t>(rand() % 32));
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LPMInstructionTests, Execute_GivenNoIncrementOrRegister_LoadsProgramAddressToRegister)
{
    auto opcode = static_cast<uint16_t>(OpCode::LPM);
    auto dst = static_cast<uint16_t>(0u);
    ctx.cpu.R[dst] = 0x00u;
    InitializeZRegister();
    auto expectedZ = *ctx.cpu.Z;
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.progMem[*ctx.cpu.Z] = expectedValue;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Z, expectedZ);
}

TEST_F(LPMInstructionTests, Execute_GivenRegister_LoadsProgramAddressToRegister)
{
    auto [opcode, dst] = GetRegisters(static_cast<uint16_t>(0x9004));
    ctx.cpu.R[dst] = 0x00u;
    InitializeZRegister();
    ctx.progMem[*ctx.cpu.Z] = static_cast<uint8_t>(rand());
    auto expectedValue = ctx.progMem[*ctx.cpu.Z];
    auto expectedZ = *ctx.cpu.Z;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Z, expectedZ);
}

TEST_F(LPMInstructionTests, Execute_GivenPostIncrement_LoadsProgramToRegisterAndIncrementsZ)
{
    auto [opcode, dst] = GetRegisters(static_cast<uint16_t>(0x9005));
    ctx.cpu.R[dst] = 0x00u;
    InitializeZRegister();
    auto expectedZ = static_cast<uint16_t>(*ctx.cpu.Z + 1u);
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.progMem[*ctx.cpu.Z] = expectedValue;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
    ASSERT_EQ(*ctx.cpu.Z, expectedZ);
}
