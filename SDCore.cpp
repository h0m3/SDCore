#include "SDCore.h"

// TODO:
// -> Write everything as static methods
// -> Add full static CRC definition
// -> Return a array with result
// -> Send a array with parameters
// -> Create a define with pin number and SD speed

bool SDCore::begin() {
    // Setup SPI
    SPI.begin();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    // Wait for SD stabilization
    SPI.beginTransaction(SPISettings(
        350000,
        MSBFIRST,
        SPI_MODE0
    ));
    for (byte i = 10; i--;)
        SPI.transfer(0xFF);
    SPI.endTransaction();

    // Send CMD0 (software reset)
    for (byte i = 255; i--;) {
        if (SDCore::command(CMD0, 0, CRC0) == 0x01)
            break;
    }

    // TODO: Add support to Ver 1.x cards
    // Send CMD8 (Check for voltage incompatible)
    if (SDCore::command(0x48, 0x1AA, 0x87) != 0x01)
        return false;

    // Send ACMD41 (Initialize newer cards)
    byte i = 255;
    do {
        if (!i)
            return false;
        i--;
        delay(1);
        SDCore::command(CMD55, 0, CRC55);
    } while (SDCore::command(CMD41, 0x4000000, 0x77));

    // Send Alternative ACMD41 (older cards)
    i = 255;
    do {
        if (!i)
            return false;
        i--;
        delay(1);
        SDCore::command(CMD55, 0, CRC55);
    } while (SDCore::command(CMD41, 0, 0xE5));

    // Send CMD1 (older cards)
    i = 255;
    do {
        if (!i)
            return false;
        i--;
        delay(1);
    } while (SDCore::command(0x41, 0, 0xF9));

    // Send CMD16 (Set sector length)
    if (SDCore::command(0x50, 512, 0xFF)) {
        return false;
    } // TODO: Calculate CRC

    return !SDCore::command(0x7B, 0, 0xFF); // TODO: Calculate CRC
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r = 0xFF;

    digitalWrite(pin, LOW);
    SPI.beginTransaction(SPISettings(
        350000,
        MSBFIRST,
        SPI_MODE0
    ));

    // Wait until device is ready
    for(byte i = 255; i--;)
        if (SPI.transfer(0xFF) == 0xFF)
            break;

    // Send command
    SPI.transfer(command);
    SPI.transfer16(param >> 16);
    SPI.transfer16(param);

    SPI.transfer(crc);

    // Wait for response
    for (byte i = 0xFF; i--;) {
        r = SPI.transfer(0xFF);
        if (r != 0xFF) {
            break;
        }
    }

    SPI.endTransaction();
    digitalWrite(pin, HIGH);

    return r;
}
