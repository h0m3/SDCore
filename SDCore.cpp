#include "SDCore.h"

bool SDCore::begin() {
    // Setup SPI
    SPI.begin();
    pinMode(SD_SS_PIN, OUTPUT);
    digitalWrite(SD_SS_PIN, HIGH);

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
    for (byte i = 255; i--;) {
        SDCore::command(CMD55, 0, CRC55);
        if (!SDCore::command(CMD41, 0x4000000, 0x77))
            goto init_complete;
        delay(1);
    }

    // Send Alternative ACMD41 (older cards)
    for (byte i = 255; i--;) {
        SDCore::command(CMD55, 0, CRC55);
        if (!SDCore::command(CMD41, 0, 0xE5))
            goto init_complete;
    }

    // Send CMD1 (older cards)
    for (byte i = 255; i--;) {
        if (!(SDCore::command(CMD1, 0, CRC1))) {
            goto init_complete;
        }
        delay(1);
    }

    return false;

    init_complete:

    // Send CMD16 (Set sector length)
    if (SDCore::command(CMD16, 512, 0xFF)) // TODO: Calculate CRC
        return false;

    return !SDCore::command(CMD59, 0, 0xFF); // TODO: Calculate CRC
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r = 0xFF;

    digitalWrite(SD_SS_PIN, LOW);
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
        if ((r = SPI.transfer(0xFF)) != 0xFF) {
            break;
        }
    }

    SPI.endTransaction();
    digitalWrite(SD_SS_PIN, HIGH);

    return r;
}
