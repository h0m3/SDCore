/*
  SDCore.cpp - SD Card Block Reading lightweight Library
  Created by Artur 'h0m3' Paiva, December 1, 2018.
  Released under LGPLv3 <https://www.gnu.org/licenses/lgpl-3.0.html>
*/
#include "SDCore.h"

bool SDCore::low_capacity = false;

// Based on SD Card Physical Layer Simplified Specification v6.00 Figure 7-2
// Download at: https://www.sdcard.org/downloads/pls/
bool SDCore::begin() {

    // Set SPI pinout
    // MISO Input / MOSI, SCK and SS output

    // Mega 1280 / Mega 2560 / Mega 32U4
    #if defined(__AVR_ATmega1280__) || \
        defined(__AVR_ATmega2560__) || \
        defined(__AVR_ATmega32U4__)

        DDRB &= 0xF7;
        DDRB |= 0x07;

        #define SS_HIGH() {PORTB |= 0x01;}
        #define SS_LOW() {PORTB &= 0xFE;}

    // Mega 644 / Mega 1284 / Mega 32 / Tiny 2313 / Tiny 4313 / Mega 16
    #elif defined(__AVR_ATmega644__) || \
        defined(__AVR_ATmega644P__) || \
        defined(__AVR_ATmega1284P__) || \
        defined(__AVR_ATmega32__) || \
        defined(__AVR_ATtiny2313__) || \
        defined(__AVR_ATtiny2313__) || \
        defined(__AVR_ATmega16__)

        DDRB &= 0xBF;
        DDRB |= 0xB0;

        #define SS_HIGH() {PORTB |= 0x10;}
        #define SS_LOW() {PORTB &= 0xEF;}

    // Tiny 25, Tiny 45, Tiny 85
    #elif defined(__AVR__ATtiny25__) || \
        defined(__AVR__ATtiny45__) || \
        defined(__AVR__ATtiny85__)

        DDRB &= 0xFD;
        DDRB |= 0x07;

        #define SS_HIGH() {PORTB |= 0x08;}
        #define SS_LOW() {PORTB &= 0xF7;}

    // Mega 168 / Mega 328 / Mega 8 / Others
    #else

        DDRB &= 0xEF;
        DDRB |= 0x2C;

        #define SS_HIGH() {PORTB |= 0x04;}
        #define SS_LOW() {PORTB &= 0xFB;}

    #endif

    // Initialize SPI Interface
    SPCR = 0x52; // ~ 300KHz, MSBFIRST, MODE0
    SS_HIGH();


    // Wait for SD power stabilization
    for (byte i = 10; --i;) {
        SPDR = 0xFF;
        while (!(SPSR & 0x80));
    }

    // Send SD Reset
    for (byte i = 255; --i;) {
        if (SDCore::command(CMD0, 0, CRC0) == 0x01) {
            break;
        }
    }

    // Check for a valid SD Card
    byte r = SDCore::command(CMD8, 0x000001AA, 0x87);
    if (r != 0x01 && r != 0x05) {
        return false;
    }

    // Get OCR
    if (SDCore::command(CMD58, 0, CRC58) != 0x01) {
        return false;
    }

    SS_LOW();

    SPDR = 0xFF;
    while (!(SPSR & 0x80));
    SDCore::low_capacity = !(SPDR && 0x40);

    SPDR = 0xFF;
    while (!(SPSR & 0x80));
    if (!(SPDR & 0x78)) {
        return false;
    }

    SS_HIGH();

    // Initialize SD Card
    for (byte i = 255;;--i) {

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

        if (!i)
            return false;
    }

    // Set block size to 512 bytes
    if (SDCore::command(CMD16, 512, 0xFF)) {
        return false;
    }

    // Change to fast mode
    SPCR = 0x52; // ~1MHz, MSBFIRST, MODE0

    // Disable CRC checking
    return !SDCore::command(CMD59, 0, 0xFF);
}


void SDCore::end() {
    // Provide 8 clock cycles
    SPDR = 0xFF;
    while (!(SPSR & 0x80));

    for (byte i = 255; --i;) {
        if (SDCore::command(CMD0, 0, CRC0) == 0x01) {
            break;
        }
    }

    SS_HIGH();
}


bool SDCore::read(unsigned long address, byte *buffer) {
    if (SDCore::low_capacity) {
        address <<= 9;
    }

    // Run command to get a block of data
    for (byte i = 255; SDCore::command(CMD17, address, 0xFF); --i) {
        if (!i) {
            return false;
        }
    }

    // Wait for data token
    SS_LOW();

    for(byte i = 255; SPDR != 0xFE; i--) {
        if (i == 0) {
            SS_HIGH();
            return false;
            SPDR = 0xFF;
            while (!(SPSR & 0x80));
        }
    }

    // Read all SD data
    for (unsigned int i = 0; i < 512; i++) {
        SPDR = 0xFF;
        while (!(SPSR & 0x80));
        buffer[i] = SPDR;
    }

    // CRC
    SPDR = 0xFF;
    while (!(SPSR & 0x80));
    SPDR = 0xFF;
    while (!(SPSR & 0x80));

    SS_HIGH();
    return true;
}


bool SDCore::write(unsigned long address, byte *buffer) {
    if (SDCore::low_capacity) {
        address <<= 9;
    }

    // Run command to write a block of data
    for (byte i = 255; SDCore::command(CMD24, address, 0xFF); --i) {
        if (!i) {
            SDCore::command(CMD12, 0, 0xFF); // TODO: Get CRC
            return false;
        }
    }

    // Send data token
    SS_LOW();
    SPDR = 0xFE;
    while (!(SPSR & 0x80));

    // Write all SD data
    for (unsigned int i = 0; i < 512; i++) {
        SPDR = buffer[i];
        while (!(SPSR & 0x80));
    }

    // CRC
    SPDR = 0xFF;
    while (!(SPSR & 0x80));
    SPDR = 0xFF;
    while (!(SPSR & 0x80));


    // SD return status
    for (byte i = 255; (SPDR & 0x1F) != 0x05; --i) {
        if (!i) {
            SS_HIGH();
            return false;
        }
        SPDR = 0xFF;
        while (!(SPSR & 0x80));
    }

    SS_HIGH();
    return true;
}


byte SDCore::command(byte command, unsigned long param, byte crc) {
    byte r;

    // Initialize SPI communication
    SS_LOW();

    // Wait for idle status
    for(byte i = 255; i--;) {
        if (SPDR == 0xFF) {
            break;
        }
        SPDR = 0xFF;
        while (!(SPSR & 0x80));
    }

    // Send command
    SPDR = command;
    while (!(SPSR & 0x80));

    SPDR = param >> 24;
    while (!(SPSR & 0x80));

    SPDR = param >> 16;
    while (!(SPSR & 0x80));

    SPDR = param >> 8;
    while (!(SPSR & 0x80));

    SPDR = param;
    while (!(SPSR & 0x80));

    SPDR = crc;
    while (!(SPSR & 0x80));

    // Wait for response
    for (byte i = 255; --i;) {
        if (r != 0xFF) {
            break;
        }

        SPSR = 0xFF;
        while (!(SPSR & 0x80));
        r = SPSR;
    }

    // End SPI communication
    SS_HIGH();
    return r;
}
