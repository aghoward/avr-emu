#include "cdif/cdif.h"
#include "core/clock.h"
#include "core/coremodule.h"
#include "core/cpu.h"
#include "core/executor.h"
#include "core/iclock.h"
#include "core/memory.h"
#include "instructions/instructionexecutor.h"

#include <memory>
#include <vector>

namespace avr
{
    void CoreModule::load(cdif::Container& ctx)
    {
        ctx.bind<Clock>().as<IClock>().in<cdif::Scope::Singleton>().build();
        ctx.bind<SRAM>().in<cdif::Scope::Singleton>().build();
        ctx.bind<ProgramMemory>().in<cdif::Scope::Singleton>().build();
        ctx.bind<CPU, SRAM&>().in<cdif::Scope::Singleton>().build();

        ctx
            .bind<Executor,
                IClock&,
                std::vector<std::unique_ptr<InstructionExecutor>>>()
            .build();
    }
}
