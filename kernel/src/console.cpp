#include "console/console.h"

#include "io/io.h"

#include "core/string.h"
#include "core/va_list.h"


static constexpr uint16_t ConsoleWidth = 80;
static constexpr uint16_t ConsoleHeight = 25;


static Console s_Console;


void InitConsole(unsigned short* baseAddress, ConsoleColor foregroundColor, ConsoleColor backgroundColor)
{
    s_Console.BaseAddress = baseAddress;
    s_Console.ForegroundColor = foregroundColor;
    s_Console.BackgroundColor = backgroundColor;
}

void SetCursorPosition(uint8_t x, uint8_t y)
{
    if (x >= ConsoleWidth) x = ConsoleWidth - 1;
    if (y >= ConsoleHeight) y = ConsoleHeight - 1;

    s_Console.CursorLocation = y * ConsoleWidth + x;

    SetCursorPosition(s_Console.CursorLocation);    
}


void SetCursorPosition(uint16_t location)
{
    if (location < 0) location = 0;
    if (location > ConsoleWidth * ConsoleHeight) location = ConsoleWidth * ConsoleHeight;

    s_Console.CursorPositionX = location % ConsoleWidth;
    s_Console.CursorPositionY = location / ConsoleWidth;

    Outport(0x3D4, 0x0F);
    Outport(0x3D5, (location & 0xFF));
    Outport(0x3D4, 0x0E);
    Outport(0x3D5, ((location >> 8) & 0xFF));
}

uint16_t GetCursorPosition()
{
    Outport(0x3D4, 0x0F);
    uint8_t lowByte = Inport(0x3D5);
    Outport(0x3D4, 0x0E);
    uint8_t highByte = Inport(0x3D5);

    return (highByte << 8) + lowByte;
}

void ClearScreen()
{
    for (unsigned int y = 0; y < 25; y++)
    {
        for (unsigned int x = 0; x < 80; x++)
        {
//            videoMemory[y * 80 + x] = 0x1F20; 
            s_Console.BaseAddress[y * ConsoleWidth + x] = (uint16_t)((MAKE_COLOR(s_Console.BackgroundColor, s_Console.ForegroundColor) << 8) + 0x20);
        }
    }

    s_Console.CursorLocation = 0;
    SetCursorPosition(s_Console.CursorLocation);
}

void PrintChar(char c)
{
    s_Console.BaseAddress[s_Console.CursorLocation++] = (uint16_t)(MAKE_COLOR(s_Console.BackgroundColor, s_Console.ForegroundColor) << 8) + c;

    SetCursorPosition(s_Console.CursorLocation);
}

void PrintString(const char* string, ...)
{
    va_list parameters;
    va_start(parameters, string);

    while (*string != 0)
    {
        char c = *string++;
        switch (c)
        {
            case 0xA:
            {
                s_Console.CursorPositionX = 0;
                s_Console.CursorPositionY++;

                SetCursorPosition(s_Console.CursorPositionX, s_Console.CursorPositionY);

                break;
            }

            case 0xD:
            {
                s_Console.CursorPositionX = 0;

                SetCursorPosition(s_Console.CursorPositionX, s_Console.CursorPositionY);

                break;
            }

            case '%':
            {
                c = *string++;

                switch (c)
                {
                    case 's':
                    {
                        const char* str = va_arg(parameters, const char*);
                        PrintString(str);

                        break;
                    }

                    case 'd':
                    {
                        uint32_t value = va_arg(parameters, uint32_t);
                        char buffer[32] = {0};
                        PrintString(IntegerToString(buffer, value, 10));

                        break;
                    }

                    case 'x':
                    {
                        uint64_t value = va_arg(parameters, uint64_t);
                        char buffer[64] = {0};
                        PrintString(HexToString(buffer, value, 16));

                        break;
                    }
                }

                break;
            }

            default:
            {
                PrintChar(c);
                break;
            }
        }
//        console->BaseAddress[console->CursorLocation++] = (uint16_t)(MAKE_COLOR(console->BackgroundColor, console->ForegroundColor) << 8) + *string++;
    }

    va_end(parameters);
}
