#include "SDCore.h"


// Constructor
SDCore::SDCore(byte ss) {
    SDCore::begin(ss);
}

// Based on SD Card Physical Layer Simplified Specification v6.00 Figure 7-2
// Download at: https://www.sdcard.org/downloads/pls/
bool SDCore::begin(byte ss) {

    // Set attributes
    pin = ss;

    // Initialize SPI
    SPI.begin();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    // Wait for SD power stabilization
    SPI.beginTransaction(low_speed);
    for (byte i = 10; i--;)
        SPI.transfer(0xFF);
    SPI.endTransaction();

    // Send SD Reset
    for (byte i = 255; i--;)
        if (SDCore::command(CMD0, 0, CRC0) == 0x01) break;

    // Check for a valid SD Card
    if (SDCore::command(CMD8, 0x000001AA, 0x87) != 0x01 && SDCore::command(CMD8, 0x000001AA, 0x87) != 0x05)
        return false;

    // Get OCR
    digitalWrite(pin, LOW);
    SPI.beginTransaction(low_speed);

    for(byte i = 255; i--;)
        if (SPI.transfer(0xFF) == 0xFF)
            break;

    SPI.transfer(CMD58);
    SPI.transfer16(0);
    SPI.transfer16(0);
    SPI.transfer(CRC58);

    for (byte i = 255; SPI.transfer(0xFF) != 0x01; i--)
        if (i == 0)
            return false;

    low_capacity = !(SPI.transfer(0xFF) && 0x40);

    if (!(SPI.transfer(0xFF) & 0x78))
        return false;

    SPI.endTransaction();
    digitalWrite(pin, HIGH);

    // Initialize SD Card
    for (byte i = 255;;i--) {

        if (SDCore::command(CMD55, 0, CRC55) == 0x05) {

            // Run CMD1 to older cards wich dont support ACMD
            if (!SDCore::command(CMD1, 0, CRC1)) break;
        } else {

            // Run ACMD41 with arg 0x40000000 for HCS cards
            if (!SDCore::command(CMD41, 0x40000000,0x77)) break;

            // Run ACMD41 for any other card
            SDCore::command(CMD55, 0, CRC55);
            if (!SDCore::command(CMD41, 0, 0xE5)) break;
        }

        if (!i) return false;
    }

    // Set block size to 512 bytes
    if (SDCore::command(CMD16, 512, 0xFF)) return false;

    // Disable CRC checking
    return !SDCore::command(CMD59, 0, 0xFF);
}


void SDCore::end() {
    SPI.transfer(0xFF); // Provide 8 clock cycles
    // TODO: Correctly shut down SD Card
    SPI.endTransaction();
    SPI.end();
    digitalWrite(pin, HIGH);
}

bool SDCore::read(unsigned long address, byte *buffer) {
    byte r;

    if (low_capacity) {
        address <<= 9;
    }

    // Run command to get a block of data
    digitalWrite(pin, LOW);
    SPI.beginTransaction(high_speed);

    for(byte i = 255; i--;)
        if ((r = SPI.transfer(0xFF)) == 0xFF)
            break;

    SPI.transfer(CMD17);
    SPI.transfer(address >> 16);
    SPI.transfer(address);
    SPI.transfer(0xFF);


    // for (unsigned int i = 0; i < 512; i++) {
    //     buffer[i] =
    // }
}

byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r;

    // Initialize SPI communication
    digitalWrite(pin, LOW);
    SPI.beginTransaction(low_speed);

    // Wait for idle status
    for(byte i = 255; i--;)
        if (SPI.transfer(0xFF) == 0xFF)
            break;

    // Send command
    SPI.transfer(command);
    SPI.transfer16(param >> 16);
    SPI.transfer16(param);
    SPI.transfer(crc);

    // Wait for response
    for (byte i = 255; i--;)
        if ((r = SPI.transfer(0xFF)) != 0xFF)
            break;

    // End SPI communication
    SPI.endTransaction();
    digitalWrite(pin, HIGH);

    return r;
}
