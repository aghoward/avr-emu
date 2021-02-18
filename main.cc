#include "cdif/cdif.h"
#include "core/clock.h"
#include "core/cpu.h"
#include "core/executor.h"
#include "core/iclock.h"
#include "core/memory.h"
#include "instructions/instructionexecutor.h"
#include "instructions/instructionmodule.h"

#include <memory>
#include <vector>

using namespace avr;

cdif::Container BuildContainer()
{
    auto ctx = cdif::Container();
    ctx.registerModule<InstructionModule>();

    ctx.bind<Clock>().as<IClock>().in<cdif::Scope::Singleton>().build();
    ctx.bind<Memory>().in<cdif::Scope::Singleton>().build();
    ctx.bind<CPU, Memory&>().in<cdif::Scope::Singleton>().build();

    ctx
        .bind<Executor,
            IClock&,
            std::vector<std::unique_ptr<InstructionExecutor>>>()
        .build();
    return ctx;
}

int main()
{
    auto container = BuildContainer();
    auto& cpu = container.resolve<CPU&>();
    auto& memory = container.resolve<Memory&>();
    auto executor = container.resolve<Executor>();
    executor.Execute(cpu, memory, 10);
    return 0;
}
