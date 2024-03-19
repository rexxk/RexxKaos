#pragma once

#include "types.h"



const char* IntegerToString(char* buffer, uint32_t value, uint8_t base);
const char* HexToString(char* buffer, uint64_t value, uint32_t length);
