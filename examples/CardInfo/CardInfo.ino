// Libraries Includes
#include <SDCore.h>

/*
    SD Card MBR Information

    This example will get MBR block
    and show some basic information
    about this card.

    This is not the optimal way to read MBR
    This example was made to be easier to read

    This is the default Arduino Uno SPI pinout
    * MOSI - Pin 11
    * MISO - Pin 12
    * CLK/SCK - Pin 13
    * SS - Pin 10
*/

void setup() {
    // Initialize serial communication at 9600 baund
    Serial.begin(9600);

    // Wait a couple of seconds (optional)
    // Allow you to get the serial comm active
    delay(2000);

    // Initialize SD Card
    byte response = SDCore::begin();

    if (!response) {
        Serial.println("SD Initialization: FAILED");
        Serial.println("Make sure than you have your wiring correct and a compatible SD card inserted.");
        return;
    }
    Serial.println("SD Initialization: SUCCESS");

    // Create an empty 512 bytes buffer for the MBR block/sector
    byte buffer[512] = {0x00};

    // Read MBR
    response = SDCore::read(0, buffer);
    if (!response) {
        Serial.println("MBR Reading: FAILED");
        return;
    }
    Serial.println("MBR Reading: SUCESS");

    // Finish SD Card communication
    SDCore::end();

    // Check if MBR is valid (MBR Signature: 0x55AA at end of the sector)
    if (buffer[0x1FE] != 0x55 || buffer[0x1FF] != 0xAA) {
        Serial.println("This SD Card doesnt use MBR");
        return;
    }

    // Show all information from MBR
    // Reference for MBR Sector: https://en.wikipedia.org/wiki/Master_boot_record
    Serial.println("\nSD Card Information:");

    // Original Physical Drive
    Serial.print("Physical Drive: ");
    Serial.println(buffer[0x0DC], HEX);

    // Timestamp
    Serial.print("Timestamp: ");
    Serial.print(buffer[0x0DD]);
    Serial.print(":");
    Serial.print(buffer[0x0DE]);
    Serial.print(":");
    Serial.println(buffer[0x0DF]);

    // Disk Signature
    Serial.print("Disk Signature: ");
    Serial.print(buffer[0x1B8], HEX);
    Serial.print(buffer[0x1B9], HEX);
    Serial.print(buffer[0x1BA], HEX);
    Serial.print(buffer[0x1BB], HEX);
    Serial.print(buffer[0x1BC], HEX);
    Serial.println(buffer[0x1BD], HEX);

    // Partition Entries
    for (byte i = 0; i < 4; i++) {

        // Calculate blocks count
        unsigned long blocks = (unsigned long)buffer[0x1CA + (i * 16)]
                             | (unsigned long)buffer[0x1CB + (i * 16)] << 8
                             | (unsigned long)buffer[0x1CC + (i * 16)] << 16
                             | (unsigned long)buffer[0x1CD + (i * 16)] << 24;

        // If its a partition with 0 blocks, its not a partition ;)
        if (blocks == 0) {
          continue;
        }

        // Partition Number
        Serial.print("Partition ");
        Serial.println(i + 1);

        // Is bootable
        Serial.print("\tBootable: ");
        if (buffer[0x1BE + (i * 16)]) {
            Serial.println("YES");
        } else {
            Serial.println("NO");
        }

        // Partition Type
        Serial.print("\tPartition Type: ");
        Serial.println(buffer[0x1C2], HEX);

        // LBA
        Serial.print("\tLBA: ");
        Serial.print(buffer[0x1C9 + (i * 16)], HEX);
        Serial.print(buffer[0x1C8 + (i * 16)], HEX);
        Serial.print(buffer[0x1C7 + (i * 16)], HEX);
        Serial.println(buffer[0x1C6 + (i * 16)], HEX);

        // Number of blocks
        Serial.print("\tBlocks: ");
        Serial.println(blocks);

        // Size
        Serial.print("\tSize: ");
        Serial.print(blocks >> 11);
        Serial.println(" MB");
    }
}

void loop() {}
