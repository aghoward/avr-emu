#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/movw.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class MOVWInstructionTests : public ::testing::Test
{

    protected:
        NoopClock clock;
        MOVWInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src >> 1u) & 0x0Fu);
            auto dstValue = static_cast<uint16_t>((dst << 3u) & 0xF0u);
            return static_cast<uint16_t>(OpCode::MOVW) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>((rand() % 28) & 0xFE);
            auto dst = static_cast<uint8_t>(src + 2);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        void StoreRegisterPair(uint8_t reg, uint16_t value) const
        {
            for (auto i = 0u; i < sizeof(value); i++)
                ctx.cpu.R[reg + i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFFu);
        }

        uint16_t ReadRegisterPair(uint8_t reg) const
        {
            auto value = static_cast<uint16_t>(0u);
            for (auto i = 0u; i < sizeof(value); i++)
                value |= static_cast<uint16_t>((ctx.cpu.R[reg + i] << (i * 8)) & 0xFFFFu);
            return value;
        }

    public:
        MOVWInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(MOVWInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(MOVWInstructionTests, Matches_GivenMOVWOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::MOVW);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(MOVWInstructionTests, Execute_CopiesRegisterPairFromSourceToDestination)
{
    auto [opcode, src, dst] = GetRegisters();
    auto expectedValue = static_cast<uint8_t>(rand());
    StoreRegisterPair(src, expectedValue);
    StoreRegisterPair(dst, static_cast<uint16_t>(rand()));

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ReadRegisterPair(src), expectedValue);
    ASSERT_EQ(ReadRegisterPair(dst), expectedValue);
}
