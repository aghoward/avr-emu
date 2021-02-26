#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/cpse.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class CPSEInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        CPSEInstruction subject;
        ExecutionContext ctx;

        uint16_t GetOpCode(uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(((src & 0x10u) << 5u) | (src & 0x0Fu));
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(OpCode::CPSE) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters()
        {
            auto src = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 31u);
            auto dst = static_cast<uint8_t>(src + 1u);
            auto compiledOpcode = GetOpCode(src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

        void WriteOpCodeToPC(OpCode opcode)
        {
            auto op = static_cast<uint16_t>(opcode);
            for (uint16_t i = 0u; i < sizeof(op); i++)
                ctx.progMem[ctx.cpu.PC + i] = static_cast<uint8_t>(op >> (i * 8u));
        }

        void TestTwoWordNextInstructionSkipsFourBytes(OpCode nextOpCode)
        {
            auto [opcode, src, dst] = GetRegisters();
            ctx.cpu.R[src] = static_cast<uint8_t>(rand());
            ctx.cpu.R[dst] = ctx.cpu.R[src];
            ctx.cpu.PC = static_cast<uint16_t>(rand()) % 0x20u;
            WriteOpCodeToPC(nextOpCode);
            auto expectedPC = static_cast<uint16_t>(ctx.cpu.PC + 4u);

            subject.Execute(opcode, ctx);

            ASSERT_EQ(ctx.cpu.PC, expectedPC);
        }

    public:
        CPSEInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(CPSEInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x3000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(CPSEInstructionTests, Matches_GivenCPSEOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CPSE);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CPSEInstructionTests, Execute_GivenRegistersNotEqual_DoesNotSkipInstruction)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = ctx.cpu.R[src] ^ 0xFFu;
    ctx.cpu.PC = static_cast<uint16_t>(rand());
    auto expectedPC = ctx.cpu.PC;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, expectedPC);
}

TEST_F(CPSEInstructionTests, Execute_GivenRegistersEqual_AndNextOpCodeSingleWord_SkipsNextInstruction)
{
    auto [opcode, src, dst] = GetRegisters();
    ctx.cpu.R[src] = static_cast<uint8_t>(rand());
    ctx.cpu.R[dst] = ctx.cpu.R[src];
    ctx.cpu.PC = static_cast<uint16_t>(rand()) % 0x20u;
    WriteOpCodeToPC(OpCode::AND);
    auto expectedPC = static_cast<uint16_t>(ctx.cpu.PC + 2u);

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.cpu.PC, expectedPC);
}

TEST_F(CPSEInstructionTests, Execute_GivenRegistersEqual_AndNextOpCodeCALL_SkipsNextInstruction)
{
    TestTwoWordNextInstructionSkipsFourBytes(OpCode::CALL);
}

TEST_F(CPSEInstructionTests, Execute_GivenRegistersEqual_AndNextOpCodeJMP_SkipsNextInstruction)
{
    TestTwoWordNextInstructionSkipsFourBytes(OpCode::JMP);
}

TEST_F(CPSEInstructionTests, Execute_GivenRegistersEqual_AndNextOpCodeLDS_SkipsNextInstruction)
{
    TestTwoWordNextInstructionSkipsFourBytes(OpCode::LDS);
}
