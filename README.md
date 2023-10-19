# avr-emu

avr-emu is a hobby project implementing the AVR instruction set as in an
emulator. It implements the full AVRe instruction set plus some of the other
extended instructions in other AVR flavors.


## But why though?

This is a hobby project. I have never taken the time to learn AVR at the
assembly language but I have plenty of the chips lying around. I usually program
these in C++; but was recently inspired to start this project by watching [Dave
Poo's YouTube videos](https://www.youtube.com/playlist?list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37)
on implementing a 6502 emulator in C++.


## Status Update

The emulator has a complete AVRe instruction set implemented. I've fudged the
ability to execute interrupt handlers. A default interrupt handler and code for
attach handlers is available inside the emulator by using the `avr::Loader`
class. This sets up the emulator with the following memory layout:

  * 0x0000 - 0x001F  Memory Mapped Registers
  * 0x0020 - 0x005F  Memory Mapped GPIO
  * 0x0060 - 0x00FF  Unused
  * 0x0100 - 0x07FF  RAM
  * 0x0900 - 0x4900  Program Memory

The first 40 bytes of program memory are occuppied by the interrupt handling
code. User code will be loaded to 0x0940 and the PC will be set to the same. The
RAM addresses 0x07F0 - 0x07FF are reserved for the interrupt handler pointers.
The SP will be automatically set to 0x07EF.

Checkout the `tests/test_executor.cc` tests for a good idea of how to get
started loading code into the emulator and executing it.
