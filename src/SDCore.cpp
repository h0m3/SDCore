/*
  SDCore.cpp - SD Card Block Reading lightweight Library
  Created by Artur 'h0m3' Paiva, December 1, 2018.
  Released under LGPLv3 <https://www.gnu.org/licenses/lgpl-3.0.html>
*/
#include "SDCore.h"


// Constructor
SDCore::SDCore(byte ss) {
    pin = ss;
}


// Based on SD Card Physical Layer Simplified Specification v6.00 Figure 7-2
// Download at: https://www.sdcard.org/downloads/pls/
bool SDCore::begin() {

    SPI.begin();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    // Wait for SD power stabilization
    SPI.beginTransaction(settings);
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
    SPI.beginTransaction(settings);

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

    for (byte i = 255; i--;)
        if (SDCore::command(CMD0, 0, CRC0) == 0x01) break;

    SPI.endTransaction();
    SPI.end();
    digitalWrite(pin, HIGH);
}


bool SDCore::read(unsigned long address, byte *buffer) {
    if (low_capacity) {
        address <<= 9;
    }

    // Run command to get a block of data
    for (byte i = 255; SDCore::command(CMD17, address, 0xFF); i--)
        if (i == 0)
            return false;

    // Wait for data token
    SPI.beginTransaction(settings);
    digitalWrite(pin, LOW);

    for(byte i = 255; SPI.transfer(0xFF) != 0xFE; i--) {
        if (i == 0) {
            SPI.endTransaction();
            digitalWrite(pin, HIGH);
            return false;
        }
    }

    // Read all SD data
    for (unsigned int i = 0; i < 512; i++) {
        buffer[i] = SPI.transfer(0xFF);
    }

    // CRC
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);

    SPI.endTransaction();
    digitalWrite(pin, HIGH);

    return true;
}


bool SDCore::write(unsigned long address, byte *buffer) {
    if (low_capacity) {
        address <<= 9;
    }

    // Run command to write a block of data
    for (byte i = 255; SDCore::command(CMD24, address, 0xFF); i--)
        if (i == 0) {
            SDCore::command(CMD12, 0, 0xFF); // TODO: Get CRC
            return false;
        }

    // Send data token
    SPI.beginTransaction(settings);
    digitalWrite(pin, LOW);

    SPI.transfer(0xFE);

    // Write all SD data
    for (unsigned int i = 0; i < 512; i++)
        SPI.transfer(buffer[i]);

    // CRC
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);

    // SD return status
    for (byte i = 255; (SPI.transfer(0xFF) & 0x1F) != 0x05; i--)
        if (i == 0) {
            SPI.endTransaction();
            digitalWrite(pin, HIGH);
            return false;
        }

    SPI.endTransaction();
    digitalWrite(pin, HIGH);

    return true;
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r;

    // Initialize SPI communication
    digitalWrite(pin, LOW);
    SPI.beginTransaction(settings);

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
