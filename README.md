# Rocketry

Rocketry Avionics Repository.

Utilises an ESP32 OR Raspberry Pi Pico as microprocessor
(Currently ESP32)

Sensors:
- MPU9250/MPU9265
  - 3dof Accel
  - 3dof Gyro
  - Temperature
- BMP280
  - Baro Pressure
  - Temperature

More to come (GPS, Datalogging to Flash/microSD, etc.)


## Install

Note this is mostly for me for new OS installs, feel free to ignore.

Install [Git on Windows](https://git-scm.com/downloads/win), NOT WSL. Use in Windows CMD/PS, otherwise use default settings

Install [Python](https://www.python.org/downloads/)

Download [VSCode](https://code.visualstudio.com/download)

Install the [PlatformIO IDE Extension](https://docs.platformio.org/en/latest/integration/ide/vscode.html)

Install C/C++ Extension Pack Extension

Test flash a microcontroller
- ESP32: Press both EN and BOOT, Release EN, then BOOT.
  - EN --> RESET