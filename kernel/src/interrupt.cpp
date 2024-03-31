#include "interrupt/interrupt.h"

#include "container/vector.h"

#include "console/console.h"




static FirmVector<InterruptDescriptor> s_InterruptVector;
static IDTR s_IDTR;

extern "C" void _SetInterruptDescriptorTable(void* address);

extern "C" void _EnableInterrupts();
extern "C" void _DisableInterrupts();

extern "C" void isr0();

extern "C" void ExceptionHandler()
{
    PrintString("Default exception handler\n");
    __asm__ volatile ("cli; hlt;");
}

extern void* isr_stub_table[];

extern "C" void DivideByZeroException()
{
    PrintString("Divide by zero\n");
}

void EnableInterrupts()
{
    _EnableInterrupts();
}

void DisableInterrupts()
{
    _DisableInterrupts();
}

void InitIDT()
{
    s_InterruptVector.SetSize();

    s_IDTR.Limit = (uint16_t)((sizeof(InterruptDescriptor) * 256) - 1);
    s_IDTR.Base = (uint64_t)s_InterruptVector.Data();

    for (uint16_t i = 0; i < 32; i++)
    {
        SetInterruptGate(i, (uint64_t)&isr_stub_table[i]);
    }

    SetInterruptGate(0, (uint64_t)&isr0);

    PrintString("IDTR address: %x\n", (uint64_t)&s_IDTR);

    PrintString("IDTR limit: %x\n", s_IDTR.Limit);
    PrintString("IDTR base: %x\n", s_IDTR.Base);

    DisableInterrupts();
    _SetInterruptDescriptorTable(&s_IDTR);
    EnableInterrupts();
}


void SetInterruptGate(uint32_t interruptEntry, uint64_t address)
{
    InterruptDescriptor& newDescriptor = s_InterruptVector.At(interruptEntry);

    newDescriptor.Attributes = 0x8E;
    newDescriptor.Selector = 0x08;
    newDescriptor.IST = 0;
    newDescriptor.OffsetLow = ((uint16_t)address & 0xFFFF);
    newDescriptor.OffsetMid = ((uint16_t)(address >> 16) & 0xFFFF);
    newDescriptor.OffsetHigh = ((uint32_t)(address >> 32) & 0xFFFFFFFF);

//    PrintString("ISR %d: Attributes: %x, offset mid: %x, offset low: %x\n", interruptEntry, newDescriptor.Attributes, newDescriptor.OffsetMid, newDescriptor.OffsetLow);
}

void SetTrapGate(uint32_t interruptEntry, uint64_t address)
{
    InterruptDescriptor& newDescriptor = s_InterruptVector.At(interruptEntry);

    newDescriptor.Attributes = 0x8F;
    newDescriptor.Selector = 0x08;
    newDescriptor.IST = 0;
    newDescriptor.OffsetLow = ((uint16_t)address & 0xFFFF);
    newDescriptor.OffsetMid = ((uint16_t)(address >> 16) & 0xFFFF);
    newDescriptor.OffsetHigh = ((uint32_t)(address >> 32) & 0xFFFFFFFF);

//    PrintString("ISR %d: Attributes: %x, offset mid: %x, offset low: %x\n", interruptEntry, newDescriptor.Attributes, newDescriptor.OffsetMid, newDescriptor.OffsetLow);
}

void SetTaskGate(uint32_t interruptEntry, uint64_t address)
{
    // TODO: Implementation
//    PrintString("ISR %d: Attributes: %x, offset mid: %x, offset low: %x\n", interruptEntry, newDescriptor.Attributes, newDescriptor.OffsetMid, newDescriptor.OffsetLow);
}
