# IS4413-M1 I2C 4-20 mA Current Loop Transmitter – Arduino Example

<p align="center">
  <a href="https://inacks.com/product/i2c-4-20-ma-current-loop-transmitter-module-3-wire-is4413-m1/">
    <img src="https://inacks.com/wp-content/uploads/IS4413.png" alt="IS4413-M1 I2C 4-20 mA current loop transmitter module" width="250">
  </a>
</p>

This example is powered by the **IS4413-M1**, an I2C current loop transmitter module (3-wire) that lets any microcontroller with I2C generate a 4-20 mA signal with a single I2C write.

No analog circuitry to design. No extra libraries. No calibration needed for general use.  
The IS4413-M1 contains the complete analog side of the transmitter: your code sends the current value over I2C, and the module generates it on its COUT pad.

Perfect for sensors and transmitters that send pressure, temperature, flow, level or weight readings to a PLC, for adding 4-20 mA outputs to your own controllers, and for driving current loop actuators without a PLC.

### 🛒 [Buy the IS4413-M1, get the datasheet and learn more](https://inacks.com/product/i2c-4-20-ma-current-loop-transmitter-module-3-wire-is4413-m1/)

---

## What this example does

- Generates 4, 8, 12, 16 and 20 mA on the current loop output, two seconds each, in an endless loop.
- Reports every step on the Serial Monitor at 115200 baud.
- Prints an error message if the IS4413-M1 does not acknowledge the I2C transfer.

The sketch defines a few small functions that you can reuse in your own projects:

| Function | What it does |
|---|---|
| `is4413Write(command, value)` | Sends the four-byte I2C write: slave address, command, Data (Hi) and Data (Lo). Returns `false` if the IS4413-M1 does not acknowledge the transfer. |
| `is4413SetValue(value)` | **Set Current Value** (command 64): updates the output current. This is the normal operation. |
| `is4413SetDefault(value)` | **Set Default Value** (command 96): updates the output current and also stores it in the internal EEPROM as the safe power-up current. |
| `is4413SetMilliamps(mA)` | Converts a current in mA into a value (0 to 4095) and sends it. |

---

## What you need

- An [IS4413-M1 module](https://inacks.com/product/i2c-4-20-ma-current-loop-transmitter-module-3-wire-is4413-m1/)
- An Arduino Uno, or any Arduino board with 5 V I2C
- A 24 V power supply that can provide at least 30 mA
- Two 4.7 kΩ resistors for the I2C pull-ups
- A current loop receiver, such as a PLC analog input. To test without a PLC, use a 250 Ω resistor (0.25 W or higher) and a multimeter.
- The Arduino IDE. The sketch only uses the standard `Wire` library, which comes with the IDE.

---

## Wiring

| IS4413-M1 pad | Connect to |
|---|---|
| 1 – SDA | Arduino SDA (A4 on the Uno), with a 4.7 kΩ pull-up to the Arduino 5V pin |
| 2 – SCL | Arduino SCL (A5 on the Uno), with a 4.7 kΩ pull-up to the Arduino 5V pin |
| 3 and 7 – GND | 24 V power supply 0 V, Arduino GND and current loop receiver return |
| 4, 5 and 6 – COUT | Current loop receiver input (to test without a PLC, a 250 Ω resistor between COUT and GND) |
| 8 – 24V | 24 V power supply + |

- The IS4413-M1 takes its power from the 24 V supply, not from the Arduino. The module, the Arduino and the receiver must share the same GND.
- Connect both GND pads and all three COUT pads.
- On other Arduino boards, use the board's SDA and SCL pins, for example pins 20 and 21 on the Arduino Mega 2560.
- The I2C interface works at 5 V. With a 3.3 V board, use an I2C level shifter unless its I2C pins are 5 V tolerant.

---

## Running the example

1. Wire the circuit and turn on the 24 V power supply.
2. Open the sketch in the Arduino IDE.
3. Select your board and port, and upload the sketch.
4. Open the Serial Monitor at 115200 baud.

You should see:

```
IS4413-M1 Arduino example
Output set to 4 mA
Output set to 8 mA
Output set to 12 mA
Output set to 16 mA
Output set to 20 mA
Output set to 4 mA
...
```

If you're testing with a 250 Ω resistor, measure the voltage across it (approximate values):

| Output current | Voltage across 250 Ω |
|:---:|:---:|
| 4 mA | 1 V |
| 8 mA | 2 V |
| 12 mA | 3 V |
| 16 mA | 4 V |
| 20 mA | 5 V |

---

## How it works

The Arduino (I2C master) sets the output current with a single four-byte write:

| Byte | Content |
|:---:|---|
| 1 | Slave address **96** (7-bit), write |
| 2 | Command: **64** (Set Current Value) or **96** (Set Default Value) |
| 3 | Data (Hi): bits D11–D4 of the 12-bit value |
| 4 | Data (Lo): bits D3–D0 in the upper four bits, lower four bits set to 0 |

The 12-bit value (0 to 4095) sets the output current from 0 mA to approximately 22 mA:

| Value | Output current (approx.) | Bytes after the address |
|:---:|:---:|:---:|
| 0 | 0 mA | 64, 0, 0 |
| 725 | 4 mA | 64, 45, 80 |
| 3640 | 20 mA | 64, 227, 128 |
| 4095 | 22 mA | 64, 255, 240 |

Besides the standard 4-20 mA range, you can generate currents below 4 mA and above 20 mA to signal saturation or fault conditions.

In the sketch, the whole I2C transfer is just this:

```cpp
bool is4413Write(uint8_t command, uint16_t value) {
  value &= 0x0FFF;                               // Keep the 12 useful bits
  Wire.beginTransmission(IS4413_ADDRESS);
  Wire.write(command);
  Wire.write((uint8_t)(value >> 4));             // Data (Hi): D11..D4
  Wire.write((uint8_t)((value & 0x0F) << 4));    // Data (Lo): D3..D0, then 0000
  return Wire.endTransmission() == 0;            // 0 means success
}
```

The same four-byte write works on any microcontroller with an I2C master interface, such as an STM32 or an ESP32.

---

## Notes

- **Accuracy:** the IS4413-M1 is ready to use, and no calibration is needed for general use. `is4413SetMilliamps()` converts mA into values using two approximate points, `VALUE_AT_4MA` (725) and `VALUE_AT_20MA` (3640). For maximum accuracy, calibrate your module: measure its output current near 4 mA and near 20 mA, calculate the values that generate exactly 4 mA and 20 mA, and replace these two constants.
- **Safe power-up current:** at power-up, the IS4413-M1 generates a stored safe current until it receives its first command. To store 0 mA as the power-up current, uncomment `is4413SetDefault(0);` in `setup()` and run the sketch once. It's only needed once, so comment it out again afterwards.
- **EEPROM writes:** `is4413SetDefault()` waits 50 ms for the EEPROM write to finish. Use it only to change the power-up current, never to update the output continuously. For normal updates, use `is4413SetValue()` or `is4413SetMilliamps()`.
- **I2C speed:** the sketch runs the bus at 100 kbps with 4.7 kΩ pull-ups. The IS4413-M1 also supports 400 kbps and 3.4 Mbps. At those speeds, use 2 kΩ pull-ups.
- **Several modules:** the I2C slave address is fixed (96). To control several IS4413-M1 modules, connect each one to a separate I2C bus or use an I2C multiplexer.

---

## Troubleshooting

If the Serial Monitor shows `Error: no answer from the IS4413-M1, check the wiring and the 24 V supply`, the module did not acknowledge the I2C transfer. Check:

- The SDA and SCL connections.
- The 4.7 kΩ pull-up resistors to 5 V.
- The 24 V power supply. The IS4413-M1 does not answer without it.
- The common GND between the Arduino, the IS4413-M1 and the 24 V power supply.

---

## Get the IS4413-M1

- 🛒 **[Buy the IS4413-M1](https://inacks.com/product/i2c-4-20-ma-current-loop-transmitter-module-3-wire-is4413-m1/)**: I2C 4-20 mA Current Loop Transmitter Module (3-Wire)
- 📄 [IS4413-M1 datasheet (ISDOC150)](https://inacks.com/wp-content/uploads/IS4413-M1-Datasheet-I2C-4-20-mA-Current-Loop-Transmitter-3-Wire-ISDOC150.pdf): electrical characteristics, pad description, I2C interface and a hardware example

ℹ️ For more information: [www.inacks.com](https://www.inacks.com)# Kappa4413USB – Python Example

Control the 4–20 mA output of an **IS4413-M1** current-loop transmitter from your PC, using the **Kappa4413USB** and a few lines of Python.

```mermaid
flowchart LR
    PC["PC (Python)"] -->|USB| K[Kappa4413USB]
    K -->|I2C| M[IS4413-M1]
    M -->|4–20 mA| L[Current loop]
```

The Kappa4413USB shows up on your PC as a serial port. It receives simple text commands over USB and converts them into I2C commands for the IS4413-M1.

## What the example does

[`ISXMPL4413ex2.py`](ISXMPL4413ex2.py):

1. Reads the Kappa4413USB firmware version.
2. Sets the IS4413-M1 power-up current to 0 mA (optional, only needed once).
3. Steps the output through 4, 8, 12, 16 and 20 mA, two seconds each.
4. Sends an out-of-range value (5000) to show how errors reported by the Kappa4413USB are handled.

## Requirements

**Hardware**

- Kappa4413USB
- IS4413-M1
- 24 V supply

**Software**

- Python 3
- [pyserial](https://pypi.org/project/pyserial/)

```bash
pip install pyserial
```

## Quick start

1. Wire the Kappa4413USB, the IS4413-M1 and the 24 V supply as shown in the Kappa4413USB User Manual, and connect the Kappa4413USB to your PC with a USB cable.
2. Find the serial port of the Kappa4413USB. This command lists the available ports:

   ```bash
   python -m serial.tools.list_ports
   ```

   On Windows it's a `COMx` port (you can also check *Device Manager → Ports (COM & LPT)*). On Linux it's usually `/dev/ttyUSB0`.

3. Set `PORT` at the top of `ISXMPL4413ex2.py`:

   ```python
   PORT = "COM3"   # Windows: "COM3", Linux: "/dev/ttyUSB0"
   ```

4. Run the example:

   ```bash
   python ISXMPL4413ex2.py
   ```

## From mA to output value

The IS4413-M1 output current is set with a value from 0 to 4095. The example converts mA to output values with a straight line through two points:

```python
VALUE_AT_4MA = 725
VALUE_AT_20MA = 3640
```

These values are approximate. For the best accuracy, calibrate your unit:

1. Connect a multimeter (mA range) in series with the current loop.
2. Send values with `set_value()` until the meter reads 4.000 mA, and put that value in `VALUE_AT_4MA`.
3. Do the same for 20.000 mA and `VALUE_AT_20MA`.

## Serial protocol

The Kappa4413USB uses 115200 baud, 8 data bits, no parity, 1 stop bit (8N1). Commands are ASCII text ending with a newline (`\n`), and every command gets a one-line reply. A reply starting with `ERR` means the command failed.

| Command | Example | Description |
|---|---|---|
| `ver` | `ver` | Returns the firmware version number |
| `<value>` | `2048` | Set Current Value: updates the output current (0–4095) |
| `<value>d` | `0d` | Set Default Value: stores the power-up current in the IS4413-M1 |

## Using the class in your own code

The `Kappa4413USB` class is self-contained. Copy it into your project together with the `VALUE_AT_4MA` and `VALUE_AT_20MA` constants, which `set_ma()` uses:

```python
kappa = Kappa4413USB("COM3")
try:
    print("Firmware version:", kappa.version())
    kappa.set_ma(12)        # 12 mA
    kappa.set_value(2048)   # raw value, 0..4095
finally:
    kappa.close()
```

> [!NOTE]
> Copy the class instead of importing `ISXMPL4413ex2.py`: importing the file also runs the demo.

| Method | Description |
|---|---|
| `version()` | Returns the firmware version number |
| `set_value(value)` | Sets the output current with a value from 0 to 4095 |
| `set_ma(ma)` | Sets the output current in mA (the result is limited to 0–4095) |
| `set_default(value)` | Stores the power-up current in the IS4413-M1 |
| `send(command)` | Sends any command and returns the reply |
| `close()` | Closes the serial port |

`send()` raises `TimeoutError` if there's no reply within 1 second, and `RuntimeError` if the reply starts with `ERR`. All the other methods go through `send()`, so they raise the same exceptions.

## Troubleshooting

| Problem | What to check |
|---|---|
| `TimeoutError: No reply...` | The `PORT` setting and the 24 V supply |
| `could not open port` | The port name, and that no other program (a serial terminal, for example) has the port open |
| `RuntimeError: ERR...` | The Kappa4413USB rejected the command, for example a value outside 0–4095 |

On Linux, a permission error usually means your user isn't in the `dialout` group. Fix it with `sudo usermod -aG dialout $USER`, then log out and back in.

## More information

The Kappa4413USB and the IS4413-M1 are INACKS products. More information at [inacks.com](https://inacks.com).
