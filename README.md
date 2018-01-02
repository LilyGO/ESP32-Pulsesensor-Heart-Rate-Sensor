# ESP32-pulsesensor-heart-rate-sensor

### ESP-IDF:https://github.com/espressif/esp-idf

### Youtube:https://www.youtube.com/watch?v=NIslndcB5DQ

![images](https://github.com/LilyGO/ESP32-pulsesensor-heart-rate-sensor/blob/master/Images/image1.jpg)

## Required Software

Get the SDK:

    git clone https://github.com/espressif/esp-idf.git
    cd esp-idf
    git submodule update --init

Set the IDF_PATH environment variable, and point it to this directory.

    export IDF_PATH=/path/to/esp-idf

Download the toolchain from: https://github.com/espressif/esp-idf#setting-up-esp-idf
You will need version 5.2.0.
Add /path/to/xtensa-esp32-elf/bin to your PATH:

    export PATH=/path/to/xtensa-esp32-elf/bin:$PATH

## Configuration

The serial port and wifi credentials are configured using make.
Type `make menuconfig` and 

* configure your serial port in `Serial flasher config` submenu
* select `Wifi Configuration` submenu
  * configure wifi credentials
* 'save', then exit
