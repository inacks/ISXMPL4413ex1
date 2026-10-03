# Kappa4413USB – Python Example

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
