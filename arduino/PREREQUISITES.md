# 🛠️ Prerequisites & Project Configuration

This document outlines the software dependencies, compatible hardware, and code architecture required to compile and run the open-source dissolved oxygen sensor (oxymeter) firmware.

---

## 1. Hardware Requirements

> **⚠️ CRITICAL WARNING:** This firmware is designed **strictly for the Arduino Due** (3.3V ARM Cortex-M3 SAM3X8E architecture). It relies heavily on low-level hardware registers (Hardware Timers `TC0`, high-speed analog-to-digital conversion `ADC`, and digital-to-analog conversion `DACC`). **It will NOT compile or run on an Arduino Uno, Mega, or Nano.**

| Component | Reference / Model | Protocol / Pin |
| :--- | :--- | :--- |
| **Microcontroller** | Arduino Due | 3.3V Logic / Native USB Port (`SerialUSB`) |
| **OLED Screen** | Midas - MDOB128064V2V-YI (128x64) | I2C (`Wire1`, 7-bit Address: `0x3C`) |
| **MicroSD Shield** | Whadda - WPI304N | SPI (CS Pin: `Pin 13`) |
| **Temperature Sensor**| TRU COMPONENTS TC-9445340 (OneWire / DS18B20) | Digital (`Pin 2`) |
| **Emission (Blue LED)** | Driven by NPN BJT transistor | Analog Output (`DAC0`) |
| **Reception (Photodiode)**| Transimpedance amplifier circuit | Analog Input (`ADC Channel 7`) |

---

## 2. Software Dependencies

Before compiling in the Arduino IDE, ensure the following external libraries are installed via the **Library Manager** (`Sketch` > `Include Library` > `Manage Libraries...`).

### External Libraries to Install:
* **`U8g2`** *(by oliver)*: Highly optimized monochrome display library (hardware I2C mode).
* **`OneWire`** *(by Paul Stoffregen)*: Digital communication protocol for the OneWire thermal probe.
* **`DallasTemperature`** *(by Miles Burton)*: High-level abstraction for precision temperature reads.

### Native Libraries (Included with Arduino SAM Boards package):
* **`SPI`**: High-speed communication for the MicroSD card shield.
* **`SD`**: FAT16/FAT32 file system management for data logging.
* **`Wire`**: I2C communication for the OLED display (utilizing the Due's secondary I2C port `Wire1`).

---

## 3. Modular Firmware Architecture

The firmware follows the Single Responsibility Principle, dividing the system into isolated, maintainable modules:

```text
├── Optode_O2_dissous.ino   # System orchestrator: main loop and measurement cycle sequencing
├── synchronous_detection.* # Low-level lock-in amplifier (Timer ISRs, DAC sine modulation, ADC reads)
├── tools.* # Mathematical models (F0 noise subtraction, Stern-Volmer, polynomial fit)
├── complex.* # Custom operator overloading for complex number arithmetic (+, -, *, /)
├── temperature.* # Non-blocking thermal reads with safe interrupt management
├── display.* # OLED visual interfaces (boot status, live results, progress bar)
└── microSD_card.* # Data logging: automatic session file creation and TSV data writing
