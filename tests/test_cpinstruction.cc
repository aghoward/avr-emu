#include "core/cpu.h"
#include "core/memory.h"
#include "core/noopclock.h"
#include "instructions/cp.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class CPInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        CPInstruction subject;
        Memory memory;
        CPU cpu;

        uint16_t GetOpCode(OpCode opcode, uint8_t src, uint8_t dst) const
        {
            auto srcValue = static_cast<uint16_t>(((src & 0x10u) << 5u) | (src & 0x0Fu));
            auto dstValue = static_cast<uint16_t>((dst & 0x1Fu) << 4u);
            return static_cast<uint16_t>(opcode) | srcValue | dstValue;
        }

        std::tuple<uint16_t, uint8_t, uint8_t> GetRegisters(OpCode opcode)
        {
            auto src = static_cast<uint8_t>(static_cast<uint8_t>(rand()) % 31u);
            auto dst = static_cast<uint8_t>(src + 1u);
            auto compiledOpcode = GetOpCode(opcode, src, dst);
            return std::make_tuple(std::move(compiledOpcode), src, dst);
        }

    public:
        CPInstructionTests() :
            clock(), subject(clock), memory(), cpu(memory)
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(CPInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(CPInstructionTests, Matches_GivenCPOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CP);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CPInstructionTests, Matches_GivenCPCOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::CPC);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(CPInstructionTests, ExecuteCP_GivenBorrowFromBit3_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    cpu.SREG.H = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.H);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenNoBorrowFromBit3_ClearsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    cpu.SREG.H = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.H);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenResultWrapsPositive_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    cpu.R[src] = static_cast<uint8_t>(-0x3u);
    cpu.SREG.V = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenResultWrapsNegative_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[dst] = static_cast<uint8_t>(0x82u);
    cpu.R[src] = static_cast<uint8_t>(0x07u);
    cpu.SREG.V = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenEqualValues_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0xFFu;
    cpu.R[src] = cpu.R[dst];
    cpu.SREG.Z = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenPositiveSourceGreaterThanPositiveDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Eu;
    cpu.R[src] = static_cast<uint8_t>(cpu.R[dst] + 1u);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenAbsoluteSourceGreaterThanAbsoluteDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    auto dstValue = static_cast<uint8_t>(rand()) % 0xFEu | 0x80u;
    auto srcValue = dstValue + 1u;
    cpu.R[dst] = static_cast<uint8_t>(dstValue);
    cpu.R[src] = static_cast<uint8_t>(srcValue);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}

TEST_F(CPInstructionTests, ExecuteCP_GivenNegativeSourceAbsoluteGreaterThanPositiveDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CP);
    auto dstValue = rand() % 0x7E;
    auto srcValue = (dstValue + 1) * -1;
    cpu.R[dst] = static_cast<uint8_t>(dstValue);
    cpu.R[src] = static_cast<uint8_t>(srcValue);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenBorrowFromBit3_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    cpu.SREG.H = false;
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.H);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenBorrowFromBit3BecauseOfCarry_SetsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x7u;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    cpu.SREG.H = false;
    cpu.SREG.C = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.H);
}


TEST_F(CPInstructionTests, ExecuteCPC_GivenNoBorrowFromBit3_ClearsHalfCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[src] = static_cast<uint8_t>(rand()) % 0x7Fu & 0xF7u;
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Fu | 0x8u;
    cpu.SREG.H = true;
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_FALSE(cpu.SREG.H);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenResultWrapsPositive_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(0x7Fu);
    cpu.R[src] = static_cast<uint8_t>(-0x3u);
    cpu.SREG.V = false;
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_TRUE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenResultWrapsNegative_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(0x82u);
    cpu.R[src] = static_cast<uint8_t>(0x07u);
    cpu.SREG.V = false;
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenResultWrapsNegativeBecauseOfCarry_SetsOverflowFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(0x82u);
    cpu.R[src] = static_cast<uint8_t>(0x02u);
    cpu.SREG.V = false;
    cpu.SREG.C = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.V);
    ASSERT_FALSE(cpu.SREG.N);
    ASSERT_FALSE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenEqualValues_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0xFFu;
    cpu.R[src] = cpu.R[dst];
    cpu.SREG.Z = false;
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenEqualValuesBecauseOfCarry_SetsZeroFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Eu + 1u;
    cpu.R[src] = static_cast<uint8_t>(cpu.R[dst] - 1u);
    cpu.SREG.Z = false;
    cpu.SREG.C = true;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.Z);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenPositiveSourceGreaterThanPositiveDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    cpu.R[dst] = static_cast<uint8_t>(rand()) % 0x7Eu;
    cpu.R[src] = static_cast<uint8_t>(cpu.R[dst] + 1u);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenAbsoluteSourceGreaterThanAbsoluteDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    auto dstValue = static_cast<uint8_t>(rand()) % 0xFEu | 0x80u;
    auto srcValue = dstValue + 1u;
    cpu.R[dst] = static_cast<uint8_t>(dstValue);
    cpu.R[src] = static_cast<uint8_t>(srcValue);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}

TEST_F(CPInstructionTests, ExecuteCPC_GivenNegativeSourceAbsoluteGreaterThanPositiveDestination_SetsCarryFlag)
{
    auto [opcode, src, dst] = GetRegisters(OpCode::CPC);
    auto dstValue = rand() % 0x7E;
    auto srcValue = (dstValue + 1) * -1;
    cpu.R[dst] = static_cast<uint8_t>(dstValue);
    cpu.R[src] = static_cast<uint8_t>(srcValue);
    cpu.SREG.C = false;

    subject.Execute(opcode, cpu, memory);

    ASSERT_TRUE(cpu.SREG.C);
}
