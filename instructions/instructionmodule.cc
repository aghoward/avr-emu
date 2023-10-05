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
#include "instructions/sbrs.h"
#include "instructions/sleep.h"
#include "instructions/spm.h"
#include "instructions/stx.h"
#include "instructions/sty.h"
#include "instructions/stz.h"
#include "instructions/sts.h"
#include "instructions/sub.h"
#include "instructions/subi.h"
#include "instructions/swap.h"
#include "instructions/xch.h"
#include "instructions/notimplemented.h"

namespace avr {
    void InstructionModule::load(cdif::Container& ctx)
    {
        ctx.bind<ADDInstruction, IClock&>()
            .build();
        ctx.bind<ADIWInstruction, IClock&>()
            .build();
        ctx.bind<ANDInstruction, IClock&>()
            .build();
        ctx.bind<ANDIInstruction, IClock&>()
            .build();
        ctx.bind<ASRInstruction, IClock&>()
            .build();
        ctx.bind<BCLRInstruction, IClock&>()
            .build();
        ctx.bind<BLDInstruction, IClock&>()
            .build();
        ctx.bind<BRBCInstruction, IClock&>()
            .build();
        ctx.bind<BREAKInstruction, IClock&>()
            .build();
        ctx.bind<BSTInstruction, IClock&>()
            .build();
        ctx.bind<CALLInstruction, IClock&>()
            .build();
        ctx.bind<CBIInstruction, IClock&>()
            .build();
        ctx.bind<COMInstruction, IClock&>()
            .build();
        ctx.bind<CPIInstruction, IClock&>()
            .build();
        ctx.bind<CPInstruction, IClock&>()
            .build();
        ctx.bind<CPSEInstruction, IClock&>()
            .build();
        ctx.bind<DECInstruction, IClock&>()
            .build();
        ctx.bind<EORInstruction, IClock&>()
            .build();
        ctx.bind<FMULInstruction, IClock&>()
            .build();
        ctx.bind<ICALLInstruction, IClock&>()
            .build();
        ctx.bind<IJMPInstruction, IClock&>()
            .build();
        ctx.bind<INInstruction, IClock&>()
            .build();
        ctx.bind<INCInstruction, IClock&>()
            .build();
        ctx.bind<JMPInstruction, IClock&>()
            .build();
        ctx.bind<LACInstruction, IClock&>()
            .build();
        ctx.bind<LASInstruction, IClock&>()
            .build();
        ctx.bind<LATInstruction, IClock&>()
            .build();
        ctx.bind<LDInstruction, IClock&>()
            .build();
        ctx.bind<LDDInstruction, IClock&>()
            .build();
        ctx.bind<LDDZInstruction, IClock&>()
            .build();
        ctx.bind<LDIInstruction, IClock&>()
            .build();
        ctx.bind<LDSInstruction, IClock&>()
            .build();
        ctx.bind<LPMInstruction, IClock&>()
            .build();
        ctx.bind<LSRInstruction, IClock&>()
            .build();
        ctx.bind<MOVInstruction, IClock&>()
            .build();
        ctx.bind<MOVWInstruction, IClock&>()
            .build();
        ctx.bind<MULInstruction, IClock&>()
            .build();
        ctx.bind<MULSInstruction, IClock&>()
            .build();
        ctx.bind<MULSUInstruction, IClock&>()
            .build();
        ctx.bind<NEGInstruction, IClock&>()
            .build();
        ctx.bind<NOPInstruction, IClock&>()
            .build();
        ctx.bind<ORInstruction, IClock&>()
            .build();
        ctx.bind<ORIInstruction, IClock&>()
            .build();
        ctx.bind<OUTInstruction, IClock&>()
            .build();
        ctx.bind<POPInstruction, IClock&>()
            .build();
        ctx.bind<PUSHInstruction, IClock&>()
            .build();
        ctx.bind<RCALLInstruction, IClock&>()
            .build();
        ctx.bind<RETInstruction, IClock&>()
            .build();
        ctx.bind<RETIInstruction, IClock&>()
            .build();
        ctx.bind<RJMPInstruction, IClock&>()
            .build();
        ctx.bind<ROLInstruction, IClock&>()
            .build();
        ctx.bind<RORInstruction, IClock&>()
            .build();
        ctx.bind<SBCInstruction, IClock&>()
            .build();
        ctx.bind<SBCIInstruction, IClock&>()
            .build();
        ctx.bind<SBIInstruction, IClock&>()
            .build();
        ctx.bind<SBICInstruction, IClock&>()
            .build();
        ctx.bind<SBISInstruction, IClock&>()
            .build();
        ctx.bind<SBIWInstruction, IClock&>()
            .build();
        ctx.bind<SBRCInstruction, IClock&>()
            .build();
        ctx.bind<SBRSInstruction, IClock&>()
            .build();
        ctx.bind<SLEEPInstruction, IClock&>()
            .build();
        ctx.bind<SPMInstruction, IClock&>()
            .build();
        ctx.bind<STXInstruction, IClock&>()
            .build();
        ctx.bind<STYInstruction, IClock&>()
            .build();
        ctx.bind<STZInstruction, IClock&>()
            .build();
        ctx.bind<STSInstruction, IClock&>()
            .build();
        ctx.bind<SUBInstruction, IClock&>()
            .build();
        ctx.bind<SUBIInstruction, IClock&>()
            .build();
        ctx.bind<SWAPInstruction, IClock&>()
            .build();
        ctx.bind<XCHInstruction, IClock&>()
            .build();
        ctx.bind<NotImplementedInstruction>()
            .build();

        ctx.bindList<std::unique_ptr<InstructionExecutor>,
                std::unique_ptr<BREAKInstruction>,
                std::unique_ptr<NOPInstruction>,
                std::unique_ptr<SLEEPInstruction>,
                std::unique_ptr<ICALLInstruction>,
                std::unique_ptr<IJMPInstruction>,
                std::unique_ptr<RETInstruction>,
                std::unique_ptr<RETIInstruction>,
                std::unique_ptr<LPMInstruction>,
                std::unique_ptr<SPMInstruction>,
                std::unique_ptr<BCLRInstruction>,
                std::unique_ptr<ASRInstruction>,
                std::unique_ptr<DECInstruction>,
                std::unique_ptr<INCInstruction>,
                std::unique_ptr<LSRInstruction>,
                std::unique_ptr<RORInstruction>,
                std::unique_ptr<SWAPInstruction>,
                std::unique_ptr<NEGInstruction>,
                std::unique_ptr<LDSInstruction>,
                std::unique_ptr<LACInstruction>,
                std::unique_ptr<LASInstruction>,
                std::unique_ptr<LATInstruction>,
                std::unique_ptr<LDDInstruction>,
                std::unique_ptr<LDDZInstruction>,
                std::unique_ptr<POPInstruction>,
                std::unique_ptr<PUSHInstruction>,
                std::unique_ptr<STSInstruction>,
                std::unique_ptr<XCHInstruction>,
                std::unique_ptr<FMULInstruction>,
                std::unique_ptr<MULSUInstruction>,
                std::unique_ptr<CALLInstruction>,
                std::unique_ptr<JMPInstruction>,
                std::unique_ptr<LDInstruction>,
                std::unique_ptr<STXInstruction>,
                std::unique_ptr<ADIWInstruction>,
                std::unique_ptr<MULSInstruction>,
                std::unique_ptr<SBIWInstruction>,
                std::unique_ptr<BLDInstruction>,
                std::unique_ptr<BSTInstruction>,
                std::unique_ptr<CBIInstruction>,
                std::unique_ptr<SBIInstruction>,
                std::unique_ptr<SBICInstruction>,
                std::unique_ptr<SBISInstruction>,
                std::unique_ptr<SBRCInstruction>,
                std::unique_ptr<SBRSInstruction>,
                std::unique_ptr<MOVWInstruction>,
                std::unique_ptr<COMInstruction>,
                std::unique_ptr<ADDInstruction>,
                std::unique_ptr<EORInstruction>,
                std::unique_ptr<MULInstruction>,
                std::unique_ptr<ROLInstruction>,
                std::unique_ptr<SBCInstruction>,
                std::unique_ptr<SUBInstruction>,
                std::unique_ptr<ANDInstruction>,
                std::unique_ptr<ORInstruction>,
                std::unique_ptr<CPInstruction>,
                std::unique_ptr<BRBCInstruction>,
                std::unique_ptr<CPSEInstruction>,
                std::unique_ptr<MOVInstruction>,
                std::unique_ptr<INInstruction>,
                std::unique_ptr<OUTInstruction>,
                std::unique_ptr<SBCIInstruction>,
                std::unique_ptr<SUBIInstruction>,
                std::unique_ptr<ANDIInstruction>,
                std::unique_ptr<ORIInstruction>,
                std::unique_ptr<CPIInstruction>,
                std::unique_ptr<RCALLInstruction>,
                std::unique_ptr<RJMPInstruction>,
                std::unique_ptr<LDIInstruction>,
                std::unique_ptr<STYInstruction>,
                std::unique_ptr<STZInstruction>,
                std::unique_ptr<NotImplementedInstruction>>()
            .build();
    }
}
