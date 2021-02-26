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
#include "instructions/cpi.h"
#include "instructions/cpse.h"
#include "instructions/dec.h"
#include "instructions/eor.h"
#include "instructions/fmul.h"
#include "instructions/icall.h"
#include "instructions/ijmp.h"
#include "instructions/in.h"
#include "instructions/inc.h"
#include "instructions/instructionexecutor.h"
#include "instructions/instructionmodule.h"
#include "instructions/jmp.h"
#include "instructions/lac.h"
#include "instructions/las.h"
#include "instructions/lat.h"
#include "instructions/ld.h"
#include "instructions/ldd.h"
#include "instructions/lddz.h"
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
        ctx.bind<CPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("CP")
            .build();
        ctx.bind<CPSEInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("CPSE")
            .build();
        ctx.bind<DECInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("DEC")
            .build();
        ctx.bind<EORInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("EOR")
            .build();
        ctx.bind<FMULInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("FMUL")
            .build();
        ctx.bind<ICALLInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ICALL")
            .build();
        ctx.bind<IJMPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("IJMP")
            .build();
        ctx.bind<INInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("IN")
            .build();
        ctx.bind<INCInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("INC")
            .build();
        ctx.bind<JMPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("JMP")
            .build();
        ctx.bind<LACInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LAC")
            .build();
        ctx.bind<LASInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LAS")
            .build();
        ctx.bind<LATInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LAT")
            .build();
        ctx.bind<LDInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LD")
            .build();
        ctx.bind<LDDInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LDD")
            .build();
        ctx.bind<LDDZInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LDDZ")
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
                std::unique_ptr<CPIInstruction>,
                std::unique_ptr<CPInstruction>,
                std::unique_ptr<CPSEInstruction>,
                std::unique_ptr<DECInstruction>,
                std::unique_ptr<EORInstruction>,
                std::unique_ptr<FMULInstruction>,
                std::unique_ptr<ICALLInstruction>,
                std::unique_ptr<IJMPInstruction>,
                std::unique_ptr<INInstruction>,
                std::unique_ptr<INCInstruction>,
                std::unique_ptr<JMPInstruction>,
                std::unique_ptr<LACInstruction>,
                std::unique_ptr<LASInstruction>,
                std::unique_ptr<LATInstruction>,
                std::unique_ptr<LDInstruction>,
                std::unique_ptr<LDDInstruction>,
                std::unique_ptr<LDDZInstruction>,
                std::unique_ptr<NotImplementedInstruction>>()
            .build();
    }
}
