#pragma once

#include "core/types.h"


struct InterruptDescriptor
{
    uint16_t OffsetLow;
    uint16_t Selector;
    uint8_t IST;
    uint8_t Attributes;
    uint16_t OffsetMid;
    uint32_t OffsetHigh;
    uint32_t Zero;
} __attribute__((packed));


struct IDTR
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed));


void EnableInterrupts();
void DisableInterrupts();

void InitIDT();

void SetInterruptGate(uint32_t interruptEntry, uint64_t address);
void SetTrapGate(uint32_t interruptEntry, uint64_t address);
void SetTaskGate(uint32_t interruptEntry, uint64_t address);
