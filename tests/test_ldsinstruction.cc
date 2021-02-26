#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/lds.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class LDSInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        LDSInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t dst) const
        {
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::LDS) | dstValue;
        }

        std::tuple<uint16_t, uint8_t> GetRegisters()
        {
            auto dst = static_cast<uint8_t>(rand() % 32);
            auto compiledOpcode = GetOpCode(dst);
            return std::make_tuple(std::move(compiledOpcode), dst);
        }

        uint16_t InitializePC()
        {
            auto k = static_cast<uint16_t>(
                (static_cast<uint16_t>(rand()) % (AVR_EMU_RAM_SIZE - ctx.cpu.SRAM_BEG))
                + ctx.cpu.SRAM_BEG);
            auto pc = static_cast<uint16_t>(
                (static_cast<uint16_t>(rand()) % AVR_EMU_FLASH_SIZE)
                - sizeof(k));
            ctx.cpu.PC = pc;
            for (auto i = 0u; i < sizeof(k); i++)
                ctx.progMem[static_cast<uint16_t>(ctx.cpu.PC + i)] = static_cast<uint8_t>((k >> (i * 8)) & 0xFF);
            return k;
        }

    public:
        LDSInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(LDSInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(LDSInstructionTests, Matches_GivenLDSOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::LDS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(LDSInstructionTests, Execute_LoadsRegisterFromDataspace)
{
    auto [opcode, dst] = GetRegisters();
    auto k = InitializePC();
    auto expectedValue = static_cast<uint8_t>(rand());
    ctx.ram[k] = expectedValue;
    ctx.cpu.R[dst] = 0x00u;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.R[dst], expectedValue);
}
