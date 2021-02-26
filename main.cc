#include "cdif/cdif.h"
#include "core/coremodule.h"
#include "core/executioncontext.h"
#include "core/executor.h"
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
    auto& ctx = container.resolve<ExecutionContext&>();
    auto executor = container.resolve<Executor>();
    executor.Execute(ctx, 10);
    return 0;
}
