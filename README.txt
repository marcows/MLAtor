Overview
========

MLAtor is an environment for using the Microchip Libraries for Applications
(MLA, formerly MAL) with desktop operating systems. Normally MLA is used in
conjunction with Microchip microcontrollers. This port allows you to work on
your project without having to deal with hardware setup, e.g. when on journey.

Dependencies:
- Legacy MLA (http://www.microchip.com/MLA), v2013-06-15 has been used;
  Current MLA (MLA >= v2013-12-20) does not work
- SDL 2.0 (http://libsdl.org/) for display output and user input

Currently working:
- Graphics Library display output for GOL objects, primitives, bitmaps and
  fonts, but not feature complete regarding the configuration, see below
- touch screen input simulated via mouse


Compiling the example or demo projects
======================================

$ cd example/ (or cd demoprojects/)
$ make MLA_INSTALL_PATH=custom_dir prepare
$ make MLA_INSTALL_PATH=custom_dir


Hints for using MLA with MLAtor
===============================

- set Compiler Type to C32/XC32 when generating resources with Graphics
  Resource Converter in "Internal Flash" format, for "External Flash" or
  "Binary" format there is no difference and C30/XC16 works as well

- external memory can be emulated by writing the content of the .hex or .bin
  resource file into the array "const unsigned char externalMemoryData[]"

- external memory resources have to be generated with start address set to 0

- only 1 external memory device is allowed, the device ID is ignored


Features
========

Implemented:
- COLOR_DEPTH 1, 4, 8, 16, 24
- USE_PALETTE
- USE_TRANSPARENT_COLOR
- Clipping
- USE_PALETTE_EXTERNAL
- USE_FONT_EXTERNAL
- USE_BITMAP_EXTERNAL
- USE_TOUCHSCREEN

Planned:
- USE_DOUBLE_BUFFERING
- USE_TRANSITION_EFFECTS
- USE_ANTIALIASED_FONTS -> GetPixel()
- USE_ALPHABLEND_LITE -> GetPixel()
- USE_ALPHABLEND
- USE_KEYBOARD
- USE_MOUSE

Supported out of the box:
- USE_NONBLOCKING_CONFIG
- USE_FOCUS
- USE_MULTIBYTECHAR
- USE_BITMAP_FLASH,
- USE_COMP_RLE
- USE_FONT_FLASH
- USE_FONT_RAM
- USE_GRADIENT
