#include "SDCore.h"

SDCore::SDCore(byte pin) {
    pinMode(pin, OUTPUT);
    _pin = pin;
}


bool SDCore::begin(byte pin) {
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

    // Send CMD0 (software reset)
    byte r = SDCore::command(0x40, 0, 0x95);
    if (r != 0x01)
        return r;

    // Send CMD8 (Check condition 0x000001AA)
    r = SDCore::command(0x48, 0x000001AA, 0x87);

    // Check for voltage incompatible cards
    if (r != 01 || SPI.transfer(0xFF) != 0x00) {
        // TODO: Add support to Ver 1.x cards
    }
    // if ( r != 0x01 || SPI.transfer(0xFF) != 0x00)
    //     return r;
    // SPI.transfer(0xFF);
    // SPI.transfer(0xFF);
    // SPI.transfer(0xFF);


}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r = 0xFF;
    digitalWrite(_pin, LOW);

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
