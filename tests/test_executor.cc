#include "cdif/cdif.h"
#include "core/coremodule.h"
#include "core/loader.h"
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
        Loader loader;
        ExecutionContext ctx;

        void LoadProgramToAddress(const char* program, std::size_t size, uint16_t address, ExecutionContext& ctx)
        {
            for (auto i = 0u; i < size; i++)
                ctx.progMem[address + i] = program[i];
        }

        void LoadProgramToAddress(const char* program, std::size_t size, uint16_t address)
        {
            LoadProgramToAddress(program, size, address, ctx);
        }

    public:
        ExecutorTests()
            : container(BuildContainer()),
            subject(container.resolve<Executor>()),
            loader(),
            ctx(loader.LoadProgram("\x88\x95"))
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

TEST_F(ExecutorTests, AttachInterrupt_PlacesInterruptHandlerInMemory)
{
    ctx.cpu.PC = 0x900u;

    ctx.cpu.R[24] = 1u;
    ctx.cpu.R[22] = 0xCD;
    ctx.cpu.R[23] = 0xAB;

    subject.Execute(ctx, 10);

    ASSERT_EQ(ctx.cpu.PC, 0x910u);
    ASSERT_EQ(*ctx.cpu.Z, 0x7F2);
    ASSERT_EQ(ctx.ram[0x7F2], 0xCD);
    ASSERT_EQ(ctx.ram[0x7F3], 0xAB);
}

TEST_F(ExecutorTests, CallInterrupt_GivenInterruptDisabled_ReturnsWithInterruptDisabled)
{
    ctx.cpu.PC = 0x912u;
    ctx.cpu.SP = ctx.ram.size() - 0x20;
    ctx.cpu.SREG.I = false;
    ctx.cpu.R[24] = 0x1;

    subject.Execute(ctx, 2);

    ASSERT_FALSE(ctx.cpu.SREG.I);
    ASSERT_EQ(ctx.cpu.PC, 0x92Cu);
}

TEST_F(ExecutorTests, CallInterrupt_GivenInterruptEnabled_CallsHandlerFromMemory)
{
    ctx.cpu.PC = 0x912u;
    ctx.cpu.SP = ctx.ram.size() - 0x20;
    ctx.cpu.SREG.I = true;
    ctx.ram[0x7F2] = 0xCD;
    ctx.ram[0x7F3] = 0xAB;
    ctx.cpu.R[24] = 0x1;

    subject.Execute(ctx, 13);

    ASSERT_EQ(ctx.cpu.PC, 0x928u);
    ASSERT_EQ(*ctx.cpu.Z, 0xABCD);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(ExecutorTests, Interrupt_GivenInterruptEnabled_ExecutesHandlerToEnd)
{
    LoadProgramToAddress(
        "\x08\xe0" // ldi     r16, 0x08       ; 8
        "\x00\x0f" // add     r16, r16
        "\x08\x95" // ret
        ,
        6,
        0x0A00
    );
    // Set interrupt handler 0
    ctx.ram[0x7F0] = 0x00;
    ctx.ram[0x7F1] = 0x0A;
    ctx.cpu.SREG.I = true;

    subject.Interrupt(ctx, 0);

    ASSERT_EQ(ctx.cpu.PC, 0x940);
    ASSERT_EQ(ctx.cpu.R[16], 16u);
    ASSERT_TRUE(ctx.cpu.SREG.I);
}

TEST_F(ExecutorTests, Interrupt_GivenInterruptDisabled_SkipsHandler)
{
    LoadProgramToAddress(
        "\x08\xe0" // ldi     r16, 0x08       ; 8
        "\x00\x0f" // add     r16, r16
        "\x08\x95" // ret
        ,
        6,
        0x0A00
    );
    // Set interrupt handler 0
    ctx.ram[0x7FE] = 0x00;
    ctx.ram[0x7FF] = 0x0A;
    ctx.cpu.SREG.I = false;
    ctx.cpu.R[16] = 0xFF;

    subject.Interrupt(ctx, 0);

    ASSERT_EQ(ctx.cpu.PC, 0x940);
    ASSERT_EQ(ctx.cpu.R[16], 0xFF);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}
