#pragma once

#include "types.h"



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
    ConsoleColor ForegroundColor = ConsoleColor::Black;
    ConsoleColor BackgroundColor = ConsoleColor::LightGray;

    uint8_t CursorPositionX = 0;
    uint8_t CursorPositionY = 0;

    uint16_t CursorLocation = 0;
};


void InitConsole(unsigned short* baseAddress, ConsoleColor foregroundColor, ConsoleColor backgroundColor);

void SetCursorPosition(uint8_t x, uint8_t y);
void SetCursorPosition(uint16_t location);
uint16_t GetCursorPosition();

void ClearScreen();

void PrintString(const char* string, ...);

