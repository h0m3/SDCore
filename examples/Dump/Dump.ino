// Libraries Includes
#include <SDCore.h>

/*
    SD Card Dump

    This example will dump the first blocks from a SD Card
    into the serial Port

    This is the default Arduino Uno SPI pinout
    * MOSI - Pin 11
    * MISO - Pin 12
    * CLK/SCK - Pin 13
    * SS - Pin 10

*/

unsigned int blocks = 10;

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

    // Create an empty buffer for data
    byte buffer[512] = {0x00};

    // Read all blocks in a loop
    for (unsigned long i = 0; i < blocks; i++) {

        // Check if the block is read correctly
        // And jump it if necessary
        if (!SDCore::read(i, buffer))
            continue;

        // Print the block content
        Serial.print("\n\nBlock ");
        Serial.println(i);

        // Loop trought each byte from block
        for (unsigned int j = 0; j < 512; j++) {
            Serial.print(buffer[j], HEX);
            Serial.print(' ');

            // Break line
            if (j && !(j % 64)) {
                Serial.println();
            }
        }
    }

    // End SD Card communication
    SDCore::end();
}

void loop() {}
