#include "cdif/cdif.h"
#include "core/coremodule.h"
#include "core/cpu.h"
#include "core/executor.h"
#include "core/memory.h"
#include "instructions/instructionmodule.h"

using namespace avr;

cdif::Container BuildContainer()
{
    auto ctx = cdif::Container();
    ctx.registerModule<CoreModule>();
    ctx.registerModule<InstructionModule>();
    return ctx;
}

int main()
{
    auto container = BuildContainer();
    auto& cpu = container.resolve<CPU&>();
    auto& progMem = container.resolve<ProgramMemory&>();
    auto& sram = container.resolve<SRAM&>();
    auto executor = container.resolve<Executor>();
    executor.Execute(cpu, progMem, sram, 10);
    return 0;
}
