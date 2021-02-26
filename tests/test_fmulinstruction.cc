#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/fmul.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class FMULInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        FMULInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(OpCode opcode, uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>((src - 0x10u) & 0x07u);
            auto dstValue = static_cast<uint16_t>(((dst - 0x10u) & 0x07u) << 0x4u);
            return static_cast<uint16_t>(opcode) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters(OpCode opcode)
        {
            auto src = static_cast<uint8_t>(rand() % 7 + 0x10);
            auto dst = static_cast<uint8_t>(src + 1);
            auto compiledOpcode = GetOpCode(opcode, src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        uint16_t GetResult() const
        {
            uint16_t result = 0x0u;
            for (auto i = 0u; i < sizeof(result); i++)
                result |= static_cast<uint16_t>(ctx.cpu.R[i] << (8u * i));
            return result;
        }

    public:
        FMULInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(FMULInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(FMULInstructionTests, Matches_GivenFMULOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::FMUL);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(FMULInstructionTests, ExecuteFMUL_GivenResultIsZero_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMUL);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = 0x0u;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_EQ(GetResult(), 0x0u);
}

TEST_F(FMULInstructionTests, ExecuteFMUL_GivenResultIsNotZero_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMUL);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand() % 5 + 1);
    ctx.cpu.R[dst] = 0x1u;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(FMULInstructionTests, ExecuteFMUL_GivenResultCausesOverflow_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMUL);
    ctx.cpu.R[src] = 0x81u;
    ctx.cpu.R[dst] = 0xFFu;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMUL_GivenResultDoesNotCauseOverflow_ClearsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMUL);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMUL_DoesAFractionalMultiply)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMUL);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    auto expectedResult = static_cast<uint16_t>((ctx.cpu.R[src] * ctx.cpu.R[dst]) << 1);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(GetResult(), expectedResult);
}

TEST_F(FMULInstructionTests, Matches_GivenFMULSOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::FMULS);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(FMULInstructionTests, ExecuteFMULS_GivenResultIsZero_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULS);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = 0x0u;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_EQ(GetResult(), 0x0u);
}

TEST_F(FMULInstructionTests, ExecuteFMULS_GivenResultIsNotZero_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULS);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand() % 5 + 1);
    ctx.cpu.R[dst] = 0x1u;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(FMULInstructionTests, ExecuteFMULS_GivenResultCausesOverflow_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULS);
    ctx.cpu.R[src] = 0x81u;
    ctx.cpu.R[dst] = 0xFFu;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMULS_GivenResultDoesNotCauseOverflow_ClearsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULS);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMULS_DoesAFractionalMultiply)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULS);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    auto expectedResult = static_cast<uint16_t>((ctx.cpu.R[src] * ctx.cpu.R[dst]) << 1);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(GetResult(), expectedResult);
}

TEST_F(FMULInstructionTests, Matches_GivenFMULSUOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::FMULSU);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(FMULInstructionTests, ExecuteFMULSU_GivenResultIsZero_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULSU);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = 0x0u;
    ctx.cpu.SREG.Z = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.Z);
    ASSERT_EQ(GetResult(), 0x0u);
}

TEST_F(FMULInstructionTests, ExecuteFMULSU_GivenResultIsNotZero_ClearsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULSU);
    ctx.cpu.R[src] = static_cast<uint8_t>(rand() % 5 + 1);
    ctx.cpu.R[dst] = 0x1u;
    ctx.cpu.SREG.Z = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.Z);
}

TEST_F(FMULInstructionTests, ExecuteFMULSU_GivenResultCausesOverflow_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULSU);
    ctx.cpu.R[src] = 0x81u;
    ctx.cpu.R[dst] = 0xFFu;
    ctx.cpu.SREG.C = false;

    subject.Execute(opcode, ctx);

    ASSERT_TRUE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMULSU_GivenResultDoesNotCauseOverflow_ClearsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULSU);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    ctx.cpu.SREG.C = true;

    subject.Execute(opcode, ctx);

    ASSERT_FALSE(ctx.cpu.SREG.C);
}

TEST_F(FMULInstructionTests, ExecuteFMULSU_DoesAFractionalMultiply)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::FMULSU);
    ctx.cpu.R[src] = 0x20u;
    ctx.cpu.R[dst] = 0x02u;
    auto expectedResult = static_cast<uint16_t>((ctx.cpu.R[src] * ctx.cpu.R[dst]) << 1);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(GetResult(), expectedResult);
}
