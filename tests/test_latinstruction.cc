#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/lat.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LATInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LATInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LAT) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 30u);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        void InitializeZRegister()
        {
            ctx.cpu.Z = (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + ctx.cpu.SRAM_BEG;
            ctx.ram[*ctx.cpu.Z] = 0x0u;
        }

    public:
        LATInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LATInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LATInstructionTests, Matches_GivenLATOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LAT);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LATInstructionTests, Execute_TogglesBitsFromDestinationInMemoryStore)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = 0x5Au;
    InitializeZRegister();
    ctx.ram[*ctx.cpu.Z] = 0xAAu;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], 0xAAu);
    ASSERT_EQ(ctx.ram[*ctx.cpu.Z], 0xF0u);
}

TEST_F(LATInstructionTests, Execute_StoresContentsOfMemoryIntoRegister)
{
    auto [opcode, dst] = GetRegisters();
    ctx.cpu.R[dst] = static_cast<uint8_t>(rand());
    InitializeZRegister();
    auto expectedRegister = static_cast<uint8_t>(rand());
    ctx.ram[*ctx.cpu.Z] = expectedRegister;
    auto expectedMemory = static_cast<uint8_t>(expectedRegister ^ ctx.cpu.R[dst]);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedRegister);
    ASSERT_EQ(ctx.ram[*ctx.cpu.Z], expectedMemory);
}
