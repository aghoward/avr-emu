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

TEST_F(ExecutorTests, AttachInterrupt_PlacesInterruptHandlerInMemory)
{
    LoadProgramToAddress(
            // attach_interrupt(index, func)
        "\x88\x0f"  // add     r24, r24
        "\xee\xef"  // ldi     r30, 0xfe       ; 0
        "\xf7\xe0"  // ldi     r31, 0x07       ; 127
        "\xe8\x1b"  // sub     r30, r24
        "\xf0\x40"  // sbci    r31, 0x00       ; 0
        "\x71\x83"  // std     Z+1, r23        ; 0x01
        "\x60\x83"  // st      Z, r22
        "\x08\x95"  // ret
        ,
        16,
        0x000);

    ctx.cpu.PC = 0x00u;
    ctx.cpu.SP = ctx.ram.size() - 0x20;

    ctx.cpu.R[24] = 1u;
    ctx.cpu.R[22] = 0xCD;
    ctx.cpu.R[23] = 0xAB;

    subject.Execute(ctx, 8);

    ASSERT_EQ(ctx.cpu.PC, 14u);
    ASSERT_EQ(*ctx.cpu.Z, AVR_EMU_RAM_SIZE - 4u);
    ASSERT_EQ(ctx.ram[AVR_EMU_RAM_SIZE - 4u], 0xCD);
    ASSERT_EQ(ctx.ram[AVR_EMU_RAM_SIZE - 3u], 0xAB);
}

TEST_F(ExecutorTests, CallInterrupt_GivenInterruptDisabled_ReturnsWithInterruptDisabled)
{
    LoadProgramToAddress(
            // handle_interrupt(index)
        "\xa7\xf4" // brid    .+20            2
        "\xf8\x94" // cli                     1
        "\x88\x0f" // add     r24, r24        1
        "\xce\xef" // ldi     r28, 0xFE       1
        "\xd7\xe0" // ldi     r29, 0x07       1
        "\xc8\x1b" // sub     r28, r24        1
        "\xd0\x40" // sbci    r29, 0x00       1
        "\xe8\x81" // ld      r30, Y          2
        "\xf9\x81" // ldd     r31, Y+1        2
        "\x09\x95" // icall                   3
        "\x18\x95" // reti                    4
        "\x08\x95"  // ret
        ,
        24,
        0x10
    );

    ctx.cpu.PC = 0x10u;
    ctx.cpu.SP = ctx.ram.size() - 0x20;
    ctx.cpu.SREG.I = false;
    ctx.cpu.R[24] = 0x1;

    subject.Execute(ctx, 2);

    ASSERT_FALSE(ctx.cpu.SREG.I);
    ASSERT_EQ(ctx.cpu.PC, 0x26u);
}

TEST_F(ExecutorTests, CallInterrupt_GivenInterruptEnabled_CallsHandlerFromMemory)
{
    LoadProgramToAddress(
            // handle_interrupt(index)
        "\xa7\xf4" // brid    .+0x20          2
        "\xf8\x94" // cli                     1
        "\x88\x0f" // add     r24, r24        1
        "\xce\xef" // ldi     r28, 0xFE       1
        "\xd7\xe0" // ldi     r29, 0x07       1
        "\xc8\x1b" // sub     r28, r24        1
        "\xd0\x40" // sbci    r29, 0x00       1
        "\xe8\x81" // ld      r30, Y          2
        "\xf9\x81" // ldd     r31, Y+1        2
        "\x09\x95" // icall                   3
        "\x18\x95" // reti                    4
        "\x08\x95"  // ret
        ,
        24,
        0x10
    );

    ctx.cpu.PC = 0x10u;
    ctx.cpu.SP = ctx.ram.size() - 0x20;
    ctx.cpu.SREG.I = true;
    ctx.ram[AVR_EMU_RAM_SIZE - 4u] = 0xCD;
    ctx.ram[AVR_EMU_RAM_SIZE - 3u] = 0xAB;
    ctx.cpu.R[24] = 0x1;

    subject.Execute(ctx, 11);

    ASSERT_EQ(ctx.cpu.PC, 0x22u);
    ASSERT_EQ(*ctx.cpu.Z, 0xABCD);
    ASSERT_FALSE(ctx.cpu.SREG.I);
}

TEST_F(ExecutorTests, Interrupt_GivenInterruptEnabled_ExecutesHandlerToEnd)
{
    auto loader = Loader();
    auto context = loader.LoadProgram("\x88\x95"); // sleep
    LoadProgramToAddress(
        "\x08\xe0" // ldi     r16, 0x08       ; 8
        "\x00\x0f" // add     r16, r16
        "\x08\x95" // ret
        ,
        6,
        0x0A00,
        context
    );
    // Set interrupt handler 0
    context.ram[0x7FE] = 0x00;
    context.ram[0x7FF] = 0x0A;
    context.cpu.SREG.I = true;

    subject.Interrupt(context, 0);

    ASSERT_EQ(context.cpu.PC, 0x940);
    ASSERT_EQ(context.cpu.R[16], 16u);
    ASSERT_TRUE(context.cpu.SREG.I);
}

TEST_F(ExecutorTests, Interrupt_GivenInterruptDisabled_SkipsHandler)
{
    auto loader = Loader();
    auto context = loader.LoadProgram("\x88\x95"); // sleep
    LoadProgramToAddress(
        "\x08\xe0" // ldi     r16, 0x08       ; 8
        "\x00\x0f" // add     r16, r16
        "\x08\x95" // ret
        ,
        6,
        0x0A00,
        context
    );
    // Set interrupt handler 0
    context.ram[0x7FE] = 0x00;
    context.ram[0x7FF] = 0x0A;
    context.cpu.SREG.I = false;
    context.cpu.R[16] = 0xFF;

    subject.Interrupt(context, 0);

    ASSERT_EQ(context.cpu.PC, 0x940);
    ASSERT_EQ(context.cpu.R[16], 0xFF);
    ASSERT_FALSE(context.cpu.SREG.I);
}
