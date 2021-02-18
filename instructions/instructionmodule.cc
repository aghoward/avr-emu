#include "cdif/cdif.h"
#include "instructions/add.h"
#include "instructions/adiw.h"
#include "instructions/and.h"
#include "instructions/andi.h"
#include "instructions/asr.h"
#include "instructions/bclr.h"
#include "instructions/bld.h"
#include "instructions/brbc.h"
#include "instructions/break.h"
#include "instructions/bst.h"
#include "instructions/call.h"
#include "instructions/instructionexecutor.h"
#include "instructions/instructionmodule.h"
#include "instructions/notimplemented.h"

namespace avr {
    void InstructionModule::load(cdif::Container& ctx)
    {
        ctx.bind<ADDInstruction>().as<InstructionExecutor>().build();
        ctx.bind<ADIWInstruction>().as<InstructionExecutor>().build();
        ctx.bind<ANDInstruction>().as<InstructionExecutor>().build();
        ctx.bind<ANDIInstruction>().as<InstructionExecutor>().build();
        ctx.bind<ASRInstruction>().as<InstructionExecutor>().build();
        ctx.bind<BCLRInstruction>().as<InstructionExecutor>().build();
        ctx.bind<BLDInstruction>().as<InstructionExecutor>().build();
        ctx.bind<BRBCInstruction>().as<InstructionExecutor>().build();
        ctx.bind<BREAKInstruction>().as<InstructionExecutor>().build();
        ctx.bind<BSTInstruction>().as<InstructionExecutor>().build();
        ctx.bind<CALLInstruction>().as<InstructionExecutor>().build();
        ctx.bind<NotImplementedInstruction>().as<InstructionExecutor>().build();

        ctx.bindList<std::unique_ptr<InstructionExecutor>,
                std::unique_ptr<ADDInstruction>,
                std::unique_ptr<ADIWInstruction>,
                std::unique_ptr<ANDInstruction>,
                std::unique_ptr<ANDIInstruction>,
                std::unique_ptr<ASRInstruction>,
                std::unique_ptr<BCLRInstruction>,
                std::unique_ptr<BLDInstruction>,
                std::unique_ptr<BRBCInstruction>,
                std::unique_ptr<BREAKInstruction>,
                std::unique_ptr<BSTInstruction>,
                std::unique_ptr<CALLInstruction>,
                std::unique_ptr<NotImplementedInstruction>>()
            .build();
    }
}
