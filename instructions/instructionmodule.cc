#include "cdif/cdif.h"
#include "core/iclock.h"
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
#include "instructions/cbi.h"
#include "instructions/com.h"
#include "instructions/cp.h"
#include "instructions/instructionexecutor.h"
#include "instructions/instructionmodule.h"
#include "instructions/notimplemented.h"

namespace avr {
    void InstructionModule::load(cdif::Container& ctx)
    {
        ctx.bind<ADDInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ADD")
            .build();
        ctx.bind<ADIWInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ADIW")
            .build();
        ctx.bind<ANDInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("AND")
            .build();
        ctx.bind<ANDIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ANDI")
            .build();
        ctx.bind<ASRInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ASR")
            .build();
        ctx.bind<BCLRInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("BCLR")
            .build();
        ctx.bind<BLDInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("BLD")
            .build();
        ctx.bind<BRBCInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("BRBC")
            .build();
        ctx.bind<BREAKInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("BREAK")
            .build();
        ctx.bind<BSTInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("BST")
            .build();
        ctx.bind<CALLInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("CALL")
            .build();
        ctx.bind<CBIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("CBI")
            .build();
        ctx.bind<COMInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("COM")
            .build();
        ctx.bind<CPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("CPI")
            .build();
        ctx.bind<NotImplementedInstruction>()
            .as<InstructionExecutor>()
            .named("NotImplemented")
            .build();

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
                std::unique_ptr<CBIInstruction>,
                std::unique_ptr<COMInstruction>,
                std::unique_ptr<CPInstruction>,
                std::unique_ptr<NotImplementedInstruction>>()
            .build();
    }
}
