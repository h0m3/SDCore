#include "SDCore.h"

SDCore::SDCore(byte pin) {
    pinMode(pin, OUTPUT);
    _pin = pin;
}


bool SDCore::begin(byte pin) {
    byte r;

    // Initialize SPI
    SPI.beginTransaction(
        SPISettings(
            400000, // 400 KHz
            MSBFIRST,
            SPI_MODE0
        )
    );

    // Wait for power stabilization
    digitalWrite(_pin, HIGH);
    for (i = 10; i--;)
        SPI.transfer(0xFF);
    digitalWrite(_pin, LOW);

    // Send CMD0 (software reset)
    for (byte i = 255; i--;) {
        r = SDCore::command(0x40, 0, 0x95);
        if (r == 0x01)
            break;
    }

    // TODO: Add support to Ver 1.x cards
    // Send CMD8 (Check for voltage incompatible)
    r = SDCore::command(0x48, 0x000001AA, 0x87);
    if (r != 0x01)
        return r;

    // Send ACMD41 (Initialize newer cards)
    for (byte i = 255; i--;) {
        SDCore::command(0x77, 0, 0x65); // CMD55
        r = SDCore::command(0x69, 0x4000000, 0x77); // CMD41
        if (!r)
            return r;
        delay(1);
    }

    // Send Alternative ACMD41 (older cards)
    for (byte i = 255; i--;) {
        SDCore::command(0x77, 0, 0x65); // CMD55
        r = SDCore::command(0x69, 0, 0xE5);
        if (!r)
            return r;
        delay(1);
    }

    // Send CMD1 (older cards)
    for (byte i = 255; i--) {
        r = SDCore::command(0x41, 0, 0xF9);
        if (!r) {
            return r;
        }
        delay(1);
    }

    // Return failed code
    return r;
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r = 0xFF;

    // Send command
    SPI.transfer(command);
    SPI.transfer(param >> 24);
    SPI.transfer(param >> 16);
    SPI.transfer(param >> 8);
    SPI.transfer(param);
    SPI.transfer(crc);

    // Wait for response
    for (byte i = 255; i--;) {
        r = SPI.transfer(0xFF);
        if (r != 0xFF)
            return r;
    }

    return r;
}
