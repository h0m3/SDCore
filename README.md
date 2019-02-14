# SDCore

This library is a lightweight and simple way to access SD Card blocks on Arduino

I didnt find any simple and updated library to do that on Arduino, so i built my own

# Installation

Click on download as zip from GitHub and import as a regular library into your Arduino IDE.

Then you can include the library by just adding the header `#include <SDCore.h>`

It also has some examples ready to use

# Features

### Its really ~~REALLY~~ lightweight

The full AVR binary (program space) is less than 1.2KB, including all dependencies and the Arduino bootloader. So you're left with almost 31KB free on a regular Arduino Uno.

The data memory is also reduced, this library will uses 10 bytes of data memory, that includes stack allocations. **Does not include the 512 bytes buffer**.

### It behaves like a regular library

It works like any other Arduino 1.5+ library. So its ready to install and use. It also comes with Examples.

### Its fast

I avoid at extreme do unecessary allocations, calls and jumps. So it works at close the speed of the SPI. Also since its under 2K the compiler can optimize to use AVR short jumps.

### Support different cards

Something that i saw in several libraries of the kind is the lack of standards so i dig up the SD Card Physical Layer Simplified Specification v6.00 from [here](https://www.sdcard.org/downloads/pls/) and follow it.

With that SDCore is able to use both SD and SDHC and also several standard sdcards that just wont work on other libraries.

> Note that SDXC are untested and may not work. They should work since they use the same standard as SDHC but who knows.

### All checks in place

If a SD Card is not supported or its malfunctioning, the SD standard will prevent it from being used. And all checkings are there. So only compatible SD Cards will work. That include like 99% of the market SD Cards.

### You can define the buffer

Instead of creating and dumping you with a 512 bytes buffer and you having no way to control memory usage, SDCore uses a pre-allocated buffer that you give and write to it, giving the memory control to you.

### Its really simple to use

No complex stuff, initialize the SD Card with one command and start to write and reading from it with only another command.

### Work with most common boards

Since most AVR Micrcontrollers share most SPI settings, its possible to support most common AVR microcontrollers. Bellow is a list of supported AVR Microcontrollers.

Microcontroller | Slave Select Pin | Tested
-|-|-
Mega 1280 | PB0 (Pin 53) | No
Mega 2650 | PB0 (Pin 53) | No
Mega 32U4 | PB0 (Pin 53) | No
Mega 644 | PB4 (Pin 4) | No
Mega 1284 | PB4 (Pin 4) | No
Mega 32 | PB4 (Pin 4) | No
Mega 16 | PB4 (Pin 4) | No
Tiny 2313 | PB4 (Pin 13) | No
Tiny 4313 | PB4 (Pin 13) | No
Tiny 25 | PB3 (Pin 3) | No
Tiny 45 | PB3 (Pin 3) | No
Tiny 85 | PB3 (Pin 3) | No
Mega 168 | PB2 (Pin 10) | No
Mega 328 | PB2 (Pin 10) | Yes
Mega 8 | PB2 (Pin 10) | No

> Since SDCore use only Hardware SPI, the pins are default for the MCU

### Custom controllers

You can setup your own different AVR microcontroller, it should only follow the AVR standard of using SPI at port B

To set your own SPI parameters you just need to use the follow constants:

```arduino
// This example define a custom board for Arduino UNO / Atmega 328

// Mask for DDRB, where 1 is output and 0 is input
// This will set MISO, MOSI, SCK and SS directions
#define SDCORE_CUSTOM_DDRB 0x2C

// This is the pin for Slave Select
// This is not the Arduino pin, this is the PORTB pin (or bit)
#define SDCORE_CUSTOM_SS 2
```

> You need to set both SDCORE_CUSTOM_SS and SDCORE_CUSTOM_DDRB to work, setting only one will have no effect

> This mode **may** be a couple of cycles slow, this is heavly depedent on your compiler optimizations

# Methods list

This is a list of methods available in SDCore class

Method | Description | Arguments | Return
-|-|-|-
`begin` | Initialize the SD Card connected | None | status (`bool`)
`end` | Close communication with that SD Card, you can remove the card after | None | None
`read` | Read a block (512 bytes) from the SD Card | address (`unsigned long`) and buffer (`byte *`) | status (`bool`)
`write` | Write a block (512 bytes) into the SD Card | address (`unsigned long`) and buffer (`byte *`) | status (`bool`)

# Template / Example

This is a simple template / example to use with SDCore

```arduino
#include <SDCore.h>

void setup() {
  // Create an empty buffer to manipulate blocks
  byte buffer[512] = { 0x00 };

  // Initialize SD Card
  SDCore::begin();

  // Read block 0 from SD
  SDCore::read(0, buffer);

  // Write block 0 into SD
  SDCore::write(0, buffer);

  // Close connection with SD
  SDCore::end();
}

void loop() {};
```

# TODO List

### Code checkings

Maybe some checkings, specialy related to DDRB are not made.

### Add SD Card information method

Create a method to get information from SD Card such as size and if its a SDHC or not.

### Complete CRC list

All necessary static CRC-7 are present, but it will make me more confortable if all possible static CRC-7 where present, since this list is built at compile time and dont use AVR resources and some non-standard card may use them. ~~win-win~~

### Test support for all boards

Since most boards arent tested yet. I need to test most boards for bugs. Specially Tiny boards since they dont have enough data memory

### Code Optimization and Cleanup

I've removed Arduino SPI library in order to make this library smaller and faster, but i think theres some optimizations to do

# License

```license
    Copyright (C) 2018  Artur 'h0m3' Paiva

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
