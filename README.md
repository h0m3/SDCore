# SDCore

This library is a lightweight and simple way to access SD Card blocks reading on Arduino

I didnt find any simple and updated library to do that on Arduino, so i built my own

# Installation

Click on download as zip from GitHub and import as a regular library into your Arduino IDE.

Then you can import the library by just adding the header `#import <SDCore.h>`

It also has some examples ready to use

# Features

### Its really ~~REALLY~~ lightweight

The full AVR binary (program space) is less than 2KB, including all dependencies and the Arduino bootloader. So you're left with 30KB free on a regular Arduino Uno.

The data memory is also reduced, this library will use less than 50 bytes of data memory, that includes stack allocations. **Does not include the 512 bytes buffer**.

### It behaves like a regular library

It works like any other Arduino 1.5+ library. So its ready to install and
use. It also comes with Examples.

### Its fast

I avoid at extreme do unecessary allocations, calls and jumps. So it works at close the speed of the SPI. Also since its under 2K the compiler can optimize to use AVR short jumps.

### Use modern SPI standards

This library use the native SPI library from Arduino, but it does using the modern calls and standards right from the documentation. So it does not block your SPI or do messy stuff, you can also ~~untested~~ run several sd cards or any other device from the SPI at the same time.

### Support different cards

Something that i saw in several libraries of the kind is the lack of standards so i dig up the SD Card Physical Layer Simplified Specification v6.00 from [here](https://www.sdcard.org/downloads/pls/) and follow it.

With that SDCore is able to use both SD and SDHC and also several standard sdcards that just wont work on other libraries.

> Note that SDXC are untested and may not work. They should work since they use the same standard as SDHC but who knows.

### All checkings in place

If a SD Card is not supported or its malfunctioning, the SD standard will prevent it from being used. And all checkings are there. So only compatible SD Cards will work. That include like 99% of the market SD Cards.

### You can define the buffer

Instead of creating and dumping you with a 512 bytes buffer and you having no way to control memory usage, SDCore uses a pre-allocated buffer that you give and write to it, giving the memory control to you.

### Its really simple to use

No complex stuff, initialize the SD Card with one command and start to write and reading from it with only another command each.

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

// Create SD object binded by SS pin
// Theres no need to insert any SD card here
SDCore sd(10);

// Create an empty buffer to manipulate blocks
byte buffer[512] = { 0x00 };

// Initialize SD Card
sd.begin();

// Read block 0 from SD
sd.read(0, buffer);

// Write block 0 into SD
sd.write(0, buffer);

// Close connection with SD
sd.end();
```

# TODO List

### Find stable speed

Now SDCore runs at 400 KHz because of unstable communication with older SD Cards and noisy SPI, find a way to increate SPI communication speed.

### Check for deconstructor in C++

Check if in any ocasion the SDCore object need to be freed from memory.

### Add SD Card information method

Create a method to get information from SD Card such as size and if its a SDHC or not.

### Add a `write` example

All examples are related to reading information from SD Card, add a example wich writes it

### Complete CRC list

All necessary static CRC-7 are present, but it will make me more confortable if all possible static CRC-7 where present, since this list is built at compile time and dont use AVR resources and some non-standard card may use them. ~~win-win~~

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
