Overview
========

MLAtor is an environment for using the Microchip Libraries for Applications
(MLA, formerly MAL) with desktop operating systems. Normally MLA is used in
conjunction with Microchip microcontrollers. This port allows you to work on
your project without having to deal with hardware setup, e.g. when on journey.

Dependencies:
- MLA (http://www.microchip.com/MLA), v2013-06-15 has been used
- SDL 2.0 (http://libsdl.org/) for display output and user input

Currently working:
- Graphics Library display output for primitives, bitmaps and fonts, but not
  feature complete regarding the configuration
- touch screen input simulated via mouse

Known problems with integrated demo projects:
- AppNotes:
  - AN1182: segmentation fault when changing to Hindi or Thai language


Compiling the example or demo projects
======================================

$ cd example/ (or cd demoprojects/)
$ make MLA_INSTALL_PATH=custom_dir prepare
$ make MLA_INSTALL_PATH=custom_dir


Hints for using MLA with MLAtor
===============================

- set Compiler Type to C32/XC32 for generating resources with Graphics Resource
  Converter
