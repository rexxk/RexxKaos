#pragma once

#include "core/types.h"

extern "C" uint8_t Inport(uint16_t address);
extern "C" void Outport(uint16_t address, uint8_t value);
