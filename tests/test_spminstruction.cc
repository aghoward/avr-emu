#include "core/executioncontext.h"
#include "core/noopclock.h"
#include "instructions/spm.h"
#include "instructions/opcodes.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

class SPMInstructionTests : public ::testing::Test
{
    protected:
        NoopClock clock;
        SPMInstruction subject;
        ExecutionContext ctx;

        std::tuple<uint16_t, uint16_t, IndirectRegister> GetRegisters()
        {
            auto offset = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
            auto compiledOpcode = static_cast<uint16_t>(OpCode::SPM);
            return std::make_tuple(compiledOpcode, offset, IndirectRegister(&ctx.cpu.R[0]));
        }


    public:
        SPMInstructionTests() :
            clock(), subject(clock), ctx()
        {
            srand(static_cast<unsigned int>(time(NULL)));
            ctx.cpu.PC = static_cast<uint16_t>(rand() % (ctx.progMem.size() / 2));
        }
};

TEST_F(SPMInstructionTests, Matches_GivenIncorrectOpCode_ReturnsFalse)
{
    auto opcode = static_cast<uint16_t>(0x1000);
    ASSERT_FALSE(subject.Matches(opcode));
}

TEST_F(SPMInstructionTests, Matches_GivenSPMOpCode_ReturnsTrue)
{
    auto opcode = static_cast<uint16_t>(OpCode::SPM);
    ASSERT_TRUE(subject.Matches(opcode));
}

TEST_F(SPMInstructionTests, Execute_StoresContentOfRegisterIntoProgramMemory)
{
    auto [opcode, offset, src] = GetRegisters();
    src = 0xA0C0;
    ctx.cpu.Z = offset;
    ctx.progMem[offset] = 0xFF;
    ctx.progMem[offset+1] = 0xFF;

    subject.Execute(opcode, ctx);

    ASSERT_EQ(ctx.progMem[offset], 0xC0);
    ASSERT_EQ(ctx.progMem[offset+1], 0xA0);
}
