#ifndef SDCORE_H
#define SDCORE_H

#include "Arduino.h"
#include <SPI.h>

class SDCore {
    public:
        SDCore(byte pin);
        byte begin(byte pin)
    private:
        byte _pin;
        byte command(byte command, unsigned long param, byte crc);
};

#endif
