cmake_minimum_required(VERSION 3.12)
### RPI Pico + Pico_LCD Barebones CMakeList
### - Get pico-sdk from https://github.com/raspberrypi/pico-sdk
### - `export PICO_SDK_PATH=/path/to/sdk`
### - Copy 'external/pico_sdk_import.cmake' from pico-sdk to this project directory
### Optional...
### `export PICO_PLATFORM=rp2040` to designate RP2040 target


### Import pico-sdk...
include(pico_sdk_import.cmake)

### Select the Pico you are building for...
set(PICO_BOARD pico)
#set(PICO_BOARD pico_w)

### Project/Target Name
project(pico_lcd_base)

### Gather source and header filenames
file(GLOB SOURCES *.c)
file(GLOB HEADERS include/*.h)

### Let the pico-sdk do it's initialisation
pico_sdk_init()

include_directories(include)
### Set target as a static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

### (OPTIONAL) Global installation for .a and headers
#install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
#install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

pico_enable_stdio_usb(pico_lcd_base 1)
pico_enable_stdio_uart(pico_lcd_base 1)

### Linker directive to include libraries (such as what you use from pico-sdk)
target_link_libraries(pico_lcd_base pico_stdlib pico_time hardware_timer hardware_pwm hardware_i2c hardware_spi hardware_adc)


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
