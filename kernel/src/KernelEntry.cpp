#include "core/types.h"

#include "io/io.h"

#include "console/console.h"
#include "memory/memory.h"

#include "interrupt/interrupt.h"

extern "C"
{



int _kernel_entry()
{
    InitConsole((unsigned short*)0xB8000, ConsoleColor::Blue, ConsoleColor::LightGray);

    ClearScreen();
    PrintString("RexxKaos operating system.\nSupported features: NONE\n", 0x10);

    GetMemoryInformation();

    InitIDT();

//    uint8_t* highPage = (uint8_t*)0x400020;
    uint8_t* highPage = (uint8_t*)0x5FFFF0;
    highPage[0] = 0x13;

//    float divideByZero = 15 / 0;


    return 0;
}




}
