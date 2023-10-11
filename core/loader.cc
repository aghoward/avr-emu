#include "core/loader.h"
#include "core/cpu.h"
#include "core/executioncontext.h"
#include "core/memory.h"

#include <memory>
#include <string>

namespace avr
{
    ExecutionContext Loader::LoadProgram(const std::string& program) const
    {
        auto memory = std::make_shared<Memory>(AVR_EMU_FLASH_SIZE + AVR_EMU_RAM_SIZE);
        auto ctx = ExecutionContext(memory, memory);

        LoadInterruptHandlers(ctx);
        LoadToMemory(ctx, program, 0x940);

        ctx.cpu.PC = 0x940;
        ctx.cpu.SP = 0x7EF;

        return ctx;
    }

    void Loader::LoadInterruptHandlers(ExecutionContext& ctx) const
    {
        auto interrupts = std::string(
            //attach_interrupt():
            "\x88\x0f" // add     r24, r24
            "\xee\xef" // ldi     r30, 0xFE       ; 254
            "\xf7\xe0" // ldi     r31, 0x07       ; 7
            "\xe8\x1b" // sub     r30, r24
            "\xf0\x40" // sbci    r31, 0x00       ; 0
            "\x71\x83" // std     Z+1, r23        ; 0x01
            "\x60\x83" // st      Z, r22
            "\x08\x95" // ret

            // handle_interrupt():
            "\x57\xf4" // brid    .+20            ; 0x26 <_handle_interrupt_end>
            "\xf8\x94" // cli
            "\x88\x0f" // add     r24, r24
            "\xce\xef" // ldi     r28, 0xFE       ; 254
            "\xd7\xe0" // ldi     r29, 0x07       ; 7
            "\xc8\x1b" // sub     r28, r24
            "\xd0\x40" // sbci    r29, 0x00       ; 0
            "\xe8\x81" // ld      r30, Y
            "\xf9\x81" // ldd     r31, Y+1        ; 0x01
            "\x09\x95" // icall
            "\x18\x95" // reti

            //_handle_interrupt_end():
            "\x08\x95" //           ret
        );

        auto progMemStart = ctx.cpu.SRAM_BEG + AVR_EMU_RAM_SIZE; // 0x900
        LoadToMemory(ctx, interrupts, progMemStart);
    }

    void Loader::LoadToMemory(ExecutionContext& ctx, const std::string& program, uint16_t address) const
    {
        for (auto i = 0; i < program.size(); i++)
            ctx.progMem[address + i] = program[i];
    }
}
