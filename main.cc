#include "cdif/cdif.h"
#include "cpu.h"
#include "executor.h"
#include "instructions/instructionexecutor.h"
#include "instructions/instructionmodule.h"
#include "memory.h"

#include <memory>
#include <vector>

using namespace avr;

cdif::Container BuildContainer()
{
    auto ctx = cdif::Container();
    ctx.registerModule<InstructionModule>();

    ctx
        .bind<Executor, std::vector<std::unique_ptr<InstructionExecutor>>>()
        .build();
    return ctx;
}

int main()
{
    auto cpu = CPU{};
    auto memory = Memory{};
    auto container = BuildContainer();
    auto executor = container.resolve<Executor>();
    executor.Execute(cpu, memory, 10);
    return 0;
}
