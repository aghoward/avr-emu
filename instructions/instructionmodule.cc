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
#include "instructions/ldi.h"
#include "instructions/lds.h"
#include "instructions/lpm.h"
#include "instructions/lsr.h"
#include "instructions/mov.h"
#include "instructions/movw.h"
#include "instructions/mul.h"
#include "instructions/muls.h"
#include "instructions/mulsu.h"
#include "instructions/neg.h"
#include "instructions/nop.h"
#include "instructions/or.h"
#include "instructions/ori.h"
#include "instructions/out.h"
#include "instructions/pop.h"
#include "instructions/push.h"
#include "instructions/rcall.h"
#include "instructions/ret.h"
#include "instructions/reti.h"
#include "instructions/rjmp.h"
#include "instructions/rol.h"
#include "instructions/ror.h"
#include "instructions/sbc.h"
#include "instructions/sbci.h"
#include "instructions/sbi.h"
#include "instructions/sbic.h"
#include "instructions/sbis.h"
#include "instructions/sbiw.h"
#include "instructions/sbrc.h"
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
        ctx.bind<LDIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LDI")
            .build();
        ctx.bind<LDSInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LDS")
            .build();
        ctx.bind<LPMInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LPM")
            .build();
        ctx.bind<LSRInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("LSR")
            .build();
        ctx.bind<MOVInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("MOV")
            .build();
        ctx.bind<MOVWInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("MOVW")
            .build();
        ctx.bind<MULInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("MUL")
            .build();
        ctx.bind<MULSInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("MULS")
            .build();
        ctx.bind<MULSUInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("MULSU")
            .build();
        ctx.bind<NEGInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("NEG")
            .build();
        ctx.bind<NOPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("NOP")
            .build();
        ctx.bind<ORInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("OR")
            .build();
        ctx.bind<ORIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ORI")
            .build();
        ctx.bind<OUTInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("OUT")
            .build();
        ctx.bind<POPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("POP")
            .build();
        ctx.bind<PUSHInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("PUSH")
            .build();
        ctx.bind<RCALLInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("RCALL")
            .build();
        ctx.bind<RETInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("RET")
            .build();
        ctx.bind<RETIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("RETI")
            .build();
        ctx.bind<RJMPInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("RJMP")
            .build();
        ctx.bind<ROLInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ROL")
            .build();
        ctx.bind<RORInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("ROR")
            .build();
        ctx.bind<SBCInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBC")
            .build();
        ctx.bind<SBCIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBCI")
            .build();
        ctx.bind<SBIInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBI")
            .build();
        ctx.bind<SBICInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBIC")
            .build();
        ctx.bind<SBISInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBIS")
            .build();
        ctx.bind<SBIWInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBIW")
            .build();
        ctx.bind<SBRCInstruction, IClock&>()
            .as<InstructionExecutor>()
            .named("SBRC")
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
                std::unique_ptr<LDIInstruction>,
                std::unique_ptr<LDSInstruction>,
                std::unique_ptr<LPMInstruction>,
                std::unique_ptr<LSRInstruction>,
                std::unique_ptr<MOVInstruction>,
                std::unique_ptr<MOVWInstruction>,
                std::unique_ptr<MULInstruction>,
                std::unique_ptr<MULSInstruction>,
                std::unique_ptr<MULSUInstruction>,
                std::unique_ptr<NEGInstruction>,
                std::unique_ptr<NOPInstruction>,
                std::unique_ptr<ORInstruction>,
                std::unique_ptr<ORIInstruction>,
                std::unique_ptr<OUTInstruction>,
                std::unique_ptr<POPInstruction>,
                std::unique_ptr<PUSHInstruction>,
                std::unique_ptr<RCALLInstruction>,
                std::unique_ptr<RETInstruction>,
                std::unique_ptr<RETIInstruction>,
                std::unique_ptr<RJMPInstruction>,
                // ADC Must Come After ROL for proper function
                // ROL RD == ADC RD, RD
                std::unique_ptr<ROLInstruction>,
                std::unique_ptr<RORInstruction>,
                std::unique_ptr<SBCInstruction>,
                std::unique_ptr<SBCIInstruction>,
                std::unique_ptr<SBIInstruction>,
                std::unique_ptr<SBICInstruction>,
                std::unique_ptr<SBISInstruction>,
                std::unique_ptr<SBIWInstruction>,
                std::unique_ptr<SBRCInstruction>,
                std::unique_ptr<NotImplementedInstruction>>()
            .build();
    }
}
