
## pico-lcd-base

Basic Raspberry Pi Pico (RP2040) project scaffolding to utilise a WaveShare Pico LCD 1.8in.
This repo will likely evolve to be more configurable so as to support general SPI-driven LCD panels.

See the `CMakeLists.txt` file for further setup instruction.


`lcd.c` is responsible for initialising and updating the LCD panel.

`surface.c` is a series of pixel array manipulation functions using a `Surface` struct which
can be passed to `lcd_draw_surface()` for output to the LCD.





## Requirements

*   pico-sdk (https://github.com/raspberrypi/pico-sdk)
*   Dependencies for pico-sdk: `cmake` `gcc-arm-non-eabi` `libnewlib-arm-none-eabi` `libstdc++-arm-none-eabi-newlib` `python3`
*   Set environment variable `PICO_SDK_PATH` to your pico-sdk directory
*   (Optionally) Set `PICO_PLATFORM` to `rp2040`
*   Copy `/path/to/pico-sdk/external/pico_sdk_import.cmake` into project base
*   If developing under Windows Subsystem for Linux, follow these extra steps:
    1.  Add the following to `/etc/wsl.conf`:
        ```
        [automount]
        options = "metadata"
        enabled = true
        ```
    2.  Restart WSL from an admin command prompt: `wsl.exe -t Ubuntu` (or `Debian`)
    3.  If CMake still fails, reboot windows



## Compiling

*   While holding the "BOOTSEL" button on the Pico, plug it in via USB.
*   Pico should show up as a USB removable drive.
*   Run the following commands to compile and link...
    ```
    mkdir build
    cd build
    cmake ..
    make -j9
    ```
*   Copy the artifact `main.uf2` to the Pico's USB drive.
*   Pico should instantly program itself and reboot, enjoy!
