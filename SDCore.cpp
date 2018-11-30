#include "SDCore.h"

SDCore::SDCore(byte cs) {
    pin = cs;
    settings = SPISettings(350000, MSBFIRST, SPI_MODE0);
}

byte SDCore::begin() {
    byte r;

    // Setup SPI
    SPI.begin();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    // Wait for SD stabilization
    SPI.beginTransaction(settings);
    for (byte i = 10; i--;)
        SPI.transfer(0xFF);
    SPI.endTransaction();

    // Send CMD0 (software reset)
    for (byte i = 255; i--;) {
        if (SDCore::command(0x40, 0, 0x95) == 0x01)
            break;
    }

    // TODO: Add support to Ver 1.x cards
    // Send CMD8 (Check for voltage incompatible)
    if ((r = SDCore::command(0x48, 0x1AA, 0x87)) != 0x01)
        return r;

    // Send ACMD41 (Initialize newer cards)
    for (byte i = 255; i--;) {
        SDCore::command(0x77, 0, 0x65);
        if (!(r = SDCore::command(0x69, 0x4000000, 0x77)))
            return r;
        delay(1);
    }

    // Send Alternative ACMD41 (older cards)
    for (byte i = 255; i--;) {
        SDCore::command(0x77, 0, 0x65);
        if (!(r = SDCore::command(0x69, 0, 0xE5)))
            return r;
        delay(1);
    }

    // Send CMD1 (older cards)
    for (byte i = 255; i--;) {
        if (!(r = SDCore::command(0x41, 0, 0xF9))) {
            return r;
        }
        delay(1);
    }

    // Return failed code
    return r;
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r = 0xFF;

    digitalWrite(pin, LOW);
    SPI.beginTransaction(settings);

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
