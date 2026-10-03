/*
 * Example: IS4413-M1 Current Loop Output Control
 *
 * This example continuously alternates the current loop output
 * between 4 mA and 20 mA every 2 seconds using the IS4413-M1 module.
 *
 * The output is updated via I2C, demonstrating basic operation of
 * the module.
 */
#include <Wire.h>

#define I2C_SLAVE_ADDRESS  96  // Address is 96 (GND) or 97 (VDD)

#define RAM     0
#define EEPROM  1

void writeDAC(uint16_t value, bool eeprom) {
    uint8_t byte1;

    value &= 0x0FFF; // Ensure the value is 12-bit (D11..D0)

    if (eeprom == true) {
      byte1 = 96; // Store value in EEPROM. This value will be loaded at power-up before any I2C command is received.
    }
    else {
      byte1 = 64; // Do not store value in EEPROM
    }

    uint8_t byte2 = (value >> 4) & 0xFF; // D11..D4
    uint8_t byte3 = (value & 0x0F) << 4; // D3..D0 followed by 4 zeros

    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(byte1);
    Wire.write(byte2);
    Wire.write(byte3);
    Wire.endTransmission();
}

void setup() {
  Wire.begin();
  
  // Store 0 in EEPROM as the default output value.
  // This ensures a safe startup by preventing any invalid output
  // while the microcontroller is still powering up.
  writeDAC(0, EEPROM);
  delay(50); // Allow 50 ms for EEPROM write cycle to complete
}

void loop() {

  // Output 4 mA:
  writeDAC(725, RAM);
  delay(4000);

  // Output 20 mA:
  writeDAC(3641, RAM);
  delay(4000);
  
}