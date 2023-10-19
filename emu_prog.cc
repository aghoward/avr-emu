typedef unsigned char uint8_t;

void attach_interrupt(uint8_t index, void (*irq) ())
{
    void** irqs = (void**)0x7F0;
    irqs[index] = (void*)irq;
}

void handle_interrupt(uint8_t index)
{
    asm("brid end");
    asm("cli");
    void** irqs = (void**)0x7F0;
    ((void (*) ())(irqs)[index])();
    asm("reti");
    asm("end: \
        ret");
}

int main() {}
