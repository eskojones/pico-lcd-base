### RPI Pico + Pico_LCD Barebones CMakeList
### - Get pico-sdk from https://github.com/raspberrypi/pico-sdk
### - `export PICO_SDK_PATH=/path/to/sdk`
### - Copy 'external/pico_sdk_import.cmake' from pico-sdk to this project directory
### Optional...
### `export PICO_PLATFORM=rp2040` to designate RP2040 target


### Import pico-sdk...
cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
set(PICO_BOARD pico)
###Pico W support if needed...
#set(PICO_BOARD pico_w)
project(Pico_LCD_Base)
pico_sdk_init()


### Add Subdirectories to include source files from here...
### e.g.: add_subdirectory(mylibs/mylibrary)


### Add include directories here...
### e.g.. include_directories(./includes/mylibrary)
include_directories(./include)

### Define the output executable...
add_executable(main 
main.c
surface.c
lcd.c
font.c
sprite.c
)


### Enable usb output, Disable UART output...
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)


### Create all output files (map/bin/hex/uf2)
pico_add_extra_outputs(main)


### Linker directive to include libraries (such as what you use from pico-sdk)
target_link_libraries(main pico_stdlib pico_time hardware_timer hardware_pwm hardware_i2c hardware_spi hardware_adc)


### Make a 'build' directory and run `cmake ..` from inside it.
### Compile and link project with `make -j9` from the 'build' directory.


### *** If CMake is failing under WSL ***
### - Edit /etc/wsl.conf and put this in it:
###   [automount]
###   options = "metadata"
###   enabled = true
### - Close WSL shells and run `wsl -t Debian` from an admin command prompt.
###   Alternatively, replace 'Debian' with 'Ubuntu'.

### *** Compilation under WSL may give 'clock skew' warnings, you can ignore these.
