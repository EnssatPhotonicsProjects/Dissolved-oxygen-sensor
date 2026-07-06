# Prerequisites & code configuration

This document outlines the software dependencies and code architecture required to compile and run the open-source dissolved oxygen sensor (oxymeter) firmware.

> **WARNING :** This firmware is designed strictly for the Arduino Due (3.3V ARM Cortex-M3 SAM3X8E architecture). It relies on low-level hardware registers (Hardware Timers `TC0`, analog-to-digital conversion `ADC` and digital-to-analog conversion `DAC`).


## Arduino dependencies

Before compiling in the Arduino IDE, ensure the following external libraries are installed via the Library Manager : <br>
(`Sketch` > `Include Library` > `Manage Libraries...`)

### External libraries to install:
* **`U8g2`** : Optimized monochrome display library (hardware I2C mode) for the OLED display.
* **`OneWire`** : Digital communication protocol for the OneWire temperature probe.
* **`DallasTemperature`** : High-level abstraction for precision temperature reads.

### Native libraries (Included with Arduino SAM Boards package):
* **`SPI`** : High-speed communication for the MicroSD card shield.
* **`SD`** : FAT16/FAT32 file system management for data logging.
* **`Wire`** : I2C communication for the OLED display (utilizing the Due's secondary I2C port `Wire1`).


## Modular architecture

The firmware follows the single responsibility principle, dividing the system into isolated and maintainable modules :

```text
├── Optode_O2_dissous.ino     # System orchestrator (main loop and measurement cycle sequencing)
├── synchronous_detection.*   # Low-level lock-in amplifier (Timer ISRs, DAC sine modulation, ADC reads)
├── tools.*                   # Mathematical models (F0 noise subtraction, Stern-Volmer, polynomial fit)
├── complex.*                 # Custom operator for complex number arithmetic (+, -, *, /)
├── temperature.*             # Thermal reads with safe interrupt management (Non-blocking)
├── display.*                 # OLED visual interfaces (boot status, live results, progress bar)
└── microSD_card.*            # Data logging (automatic session file creation and TSV data writing)
