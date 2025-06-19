# Rocketry

Rocketry Avionics Repository.

Utilises an ESP32 OR Raspberry Pi Pico as microprocessor
(Currently ESP32)

Sensors:
- MPU9250/MPU6500
  - 3dof Accel
  - 3dof Gyro
  - Temperature
- BMP280
  - Baro Pressure
  - Temperature
- SD Card
  - 16/32GB microSD card, FAT32 formatted.
- W25Q128
  - SPI Flash Chip

More to come (GPS, Datalogging to Flash/microSD, etc.)


## Install

Note this is mostly for me for new OS installs, feel free to ignore.

1. Install [Git on Windows](https://git-scm.com/downloads/win), NOT WSL. Use in Windows CMD/PS, otherwise use default settings

2. Install [Python](https://www.python.org/downloads/)

3. Download [VSCode](https://code.visualstudio.com/download)

4. Install the [PlatformIO IDE Extension](https://docs.platformio.org/en/latest/integration/ide/vscode.html)

5. Install C/C++ Extension Pack Extension

Test flash a microcontroller
- ESP32:
  - Press both EN and BOOT, Release EN, then BOOT.
  - EN --> RESET
- Pi Pico:
  - Install drivers using `zadig.exe`
  - Unplug. Hold BOOTSEL down and plug in. Release BOOTSEL after a few seconds.