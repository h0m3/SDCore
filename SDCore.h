#ifndef SDCORE_H
#define SDCORE_H

#include "Arduino.h"
#include <SPI.h>

// SD Card Speed
#ifndef SD_SPEED
#define SD_SPEED 20000000
#endif

// SD Card SS Pin
// TODO: Check for correct MCU pin
#ifndef SD_SS_PIN
#define SD_SS_PIN 10
#endif

// SD Commands Definition
#define CMD0 0x40
#define CMD1 0x41
#define CMD8 0x48
#define CMD9 0x49
#define CMD10 0x4A
#define CMD12 0x4C
#define CMD16 0x50
#define CMD17 0x51
#define CMD18 0x52
#define CMD23 0x57
#define CMD24 0x58
#define CMD25 0x59
#define CMD41 0x69
#define CMD55 0x77
#define CMD58 0x7A
#define CMD59 0x7B

// SD Commands CRC
#define CRC0 0x95
#define CRC1 0xF9
#define CRC8 0x87
#define CRC55 0x65

class SDCore {
    public:
        static bool begin();
    private:
        static byte command(byte command, unsigned long param, byte crc);
};

#endif
