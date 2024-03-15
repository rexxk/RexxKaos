#include "include/types.h"

#include "include/io.h"


extern "C"
{


enum class ConsoleColor
{
    Black = 0,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGray,
    DarkGray,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    Yellow,
    White,
};

#define MAKE_COLOR(background, foreground) (uint8_t)((((uint8_t)background & 0xF) << 4) | ((uint8_t)foreground & 0xF))

struct Console
{
    uint16_t* BaseAddress = 0;
    uint8_t ForegroundColor = (uint8_t)ConsoleColor::Black;
    uint8_t BackgroundColor = (uint8_t)ConsoleColor::LightGray;

    uint16_t CursorLocation = 0;
};


void PrintString(Console* console, const char* string)
{
//    uint16_t* address = (uint16_t*)0xB8000;

//    uint16_t location = 0;
//    address[0] = (uint16_t)(0x70 << 8) + (*string);

    while (*string != 0)
    {
//        address[location++] = (uint16_t)(MAKE_COLOR(ConsoleColor::LightRed, ConsoleColor::Black) << 8) + *string++;
        console->BaseAddress[console->CursorLocation++] = (uint16_t)(MAKE_COLOR(ConsoleColor::LightGray, ConsoleColor::Magenta) << 8) + *string++;
    }
}


void ClearScreen()
{
    uint16_t* videoMemory = (uint16_t*)0xB8000;

    for (unsigned int y = 0; y < 25; y++)
    {
        for (unsigned int x = 0; x < 80; x++)
        {
//            videoMemory[y * 80 + x] = 0x1F20; 
            uint16_t value = (uint16_t)((0x1F) << 8) + 'A';
            videoMemory[y * 80 + x] = value;
            videoMemory[y * 80 + x] = (uint16_t)((MAKE_COLOR(ConsoleColor::Red, ConsoleColor::LightBlue) << 8) + 0x20);
        }
    }
}

int _kernel_entry()
{
    uint8_t value = Inport(0x64);
    Outport(0x64, 0xF0);

    Console console = {};
    console.BaseAddress = (uint16_t*)0xB8000;

//    ClearScreen();
    PrintString(&console, "RexxKaos operating system. Very much just for testing at the moment...");

    return 0;
}


}
