Simple blinky app for SiLabs BRD4314a (EK4314a) using Gecko SDK, cmake
and a perl script for creating the SDK cmake files.

The app also demonstrates stdio via the USB VCOM port and how
the sl_iostream_retarget_stdio.c low-level functions are linked before
C library empty stubs.

This project provides a cmake based project template for building
embedded software for EFR32 microcontrollers from Silicon Labs.
It is heavily based on https://github.com/RoboGnome/efr32_base.git
and uses the Gecko SDK project by creating additional cmake files for
for the sdk. The major difference from RoboGnomes code is that the Gecko
SDK is not copied into the repo tree. You need to download the SDK
from https://github.com/SiliconLabs/gecko_sdk.git

Tested only on Linux.

To build:

1. install the efr32_tools module
  git submodule update

2. install the Segger tools and add <segger path> into $PATH

2. set environment variables for the GNU tool chain and Gecko SDK
  export ARM_TOOLCHAIN=<path to GNU arm tools>
  export GECKO_SDK=<path to Gecko SDK>

3. cd into build directory

3. run cmake and make
  cmake ..
  make

5. plug the BRD4314a into a usb port and upload the firmware
  make flash


