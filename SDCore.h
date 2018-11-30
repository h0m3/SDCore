#ifndef SDCORE_H
#define SDCORE_H

#include "Arduino.h"
#include <SPI.h>

class SDCore {
    public:
        SDCore(byte cs);
        byte begin();
    private:
        byte pin;
        SPISettings settings;
        byte command(byte command, unsigned long param, byte crc);
};

#endif
