/*
  SDCore.h - SD Card Block Reading lightweight Library
  Created by Artur 'h0m3' Paiva, December 1, 2018.
  Released under LGPLv3 <https://www.gnu.org/licenses/lgpl-3.0.html>
*/

#ifndef SDCORE_H
#define SDCORE_H

#include "Arduino.h"

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
// TODO: Complete CRC list
#define CRC0 0x95
#define CRC1 0xF9
#define CRC55 0x65
#define CRC58 0x95


class SDCore {
    public:
        static bool begin();
        static void end();
        static bool read(unsigned long address, byte *buffer);
        static bool write(unsigned long address, byte *buffer);
    private:
        static bool low_capacity;
        static byte command(byte command, unsigned long param, byte crc);
};

#endif
