#include "io.h"

extern "C" uint8_t _inport(uint16_t address);
extern "C" void _outport(uint16_t address, uint8_t value);

uint8_t Inport(uint16_t address)
{
    return _inport(address);
}

void Outport(uint16_t address, uint8_t value)
{
    _outport(address, value);
}
