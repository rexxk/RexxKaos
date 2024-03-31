#include "core/types.h"

#include "io/io.h"

#include "console/console.h"
#include "memory/memory.h"
#include "memory/PhysicalMemory.h"

#include "interrupt/interrupt.h"

extern "C"
{



int _kernel_entry()
{
    InitConsole((unsigned short*)0xB8000, ConsoleColor::Blue, ConsoleColor::LightGray);

    ClearScreen();
    PrintString("RexxKaos operating system.\nSupported features: NONE\n", 0x10);

    MemoryInformation memoryInformation = GetMemoryInformation();

    InitIDT();

    PhysicalMemory physicalMemory(memoryInformation.TotalMemorySize);


//    uint8_t* highPage = (uint8_t*)0x400020;
    uint8_t* highPage = (uint8_t*)0x1FFFFF0;
    highPage[0] = 0x13;

//    float divideByZero = 15 / 0;


    return 0;
}




}
