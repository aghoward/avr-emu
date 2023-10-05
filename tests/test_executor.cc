#include "cdif/cdif.h"
#include "core/coremodule.h"
#include "core/executioncontext.h"
#include "core/executor.h"
#include "core/noopclock.h"
#include "instructions/instructionmodule.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace avr;

cdif::Container BuildContainer()
{
    auto ctx = cdif::Container();
    ctx.registerModule<InstructionModule>();
    ctx.registerModule<CoreModule>();
    return ctx;
}

class ExecutorTests : public ::testing::Test
{

    protected:
        cdif::Container container;
        Executor subject;
        ExecutionContext ctx;

        void LoadProgramToAddress(const char* program, std::size_t size, uint16_t address)
        {
            for (auto i = 0u; i < size; i++)
                ctx.progMem[address + i] = program[i];
        }

    public:
        ExecutorTests()
            : container(BuildContainer()),
            subject(container.resolve<Executor>()),
            ctx(container.resolve<ExecutionContext>())
        {
            srand(static_cast<unsigned int>(time(NULL)));
        }
};

TEST_F(ExecutorTests, Execute_GivenEnoughCycles_CompletesProgram)
{
    LoadProgramToAddress(
                // add():
        "\x1f\x92" // push r1           2
        "\x12\x1c" // ADC  r1, r2       1
        "\x31\x2c" // mov  r3, r1       1
        "\x1f\x90" // pop  r1           2
        "\x08\x95" // ret               4
                // main():
        "\x05\xe0" // ldi r16, 0x05     1
        "\x19\xe0" // ldi r17, 0x09     1
        "\x10\x2e" // mov r1, r16       1
        "\x21\x2e" // mov r2, r17       1
        "\xf6\xdf" // rcall .-20        3
        ,
        20,
        0x100);
    ctx.cpu.PC = 0x10a;
    ctx.cpu.SP = ctx.cpu.SRAM_BEG + 0x100u;
    auto originalSP = ctx.cpu.SP;

    subject.Execute(ctx, 17);

    ASSERT_EQ(ctx.cpu.PC, 0x114u);
    ASSERT_EQ(ctx.cpu.SP, originalSP);
    ASSERT_EQ(ctx.cpu.R[3], 14u);
}
