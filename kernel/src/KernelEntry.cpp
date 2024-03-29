#include "core/types.h"

#include "io/io.h"

#include "console/console.h"
#include "memory/memory.h"


extern "C"
{



int _kernel_entry()
{
    InitConsole((unsigned short*)0xB8000, ConsoleColor::Blue, ConsoleColor::LightGray);

    ClearScreen();
    PrintString("RexxKaos operating system.\nSupported features: NONE\n", 0x10);

    GetMemoryInformation();

    return 0;
}




}
