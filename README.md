# H7oscillo
 A dual-channel digital storage oscilloscope built around the STM32F746G discovery board.

Features:
1) The 32F746GDISCOVERY board is used for this project without any modification.
2) Implemented using only the STM32F7 Low-level drivers and direct register accesses.
3) Oscilloscope Channel 1 is Arduino-connector A0, Channel 2 is Arduino-connector A1. 
4) Single quadrature encoder is used to change various oscilloscope parameters. Data1 output of the quadrature encoder is connected to Arduino connector D1, and Data2 output is connected to Arduino connector D0. The push button output of the quadrature encoder, which is used to select a particular field for changing, is connected to Arduino connector D2.
5) The touchscreen functionality is used to select menus, etc.
6) The GUI for the oscilloscope is built using the µGUI v0.31 embedded graphics library: https://github.com/achimdoebler/UGUI. 
7) Uses 8 bits per sample. The oscilloscope uses a sample memory of 1.4K samples, and sample rates ranging from 48KHz to 2.22MHz depending on the time scale.
8) The input signal voltage range is 0 - 3.3V (STM32 inbuilt ADC).
9) Three trigger modes: Auto, Single, Normal. Sensitive on Rising, Falling or both the edges.
10) Up to 4 measurements can be made on the waveforms at a time: frequency, duty cycle, RMS, max, min, peak-peak, average.
11) Zoom-in/out on the captured waveform for analysis.
12) Three channel waveform display modes: Split, Merged, Single (channel 1 only).
13) FFT mode for spectrum analysis (480-point, 0.15 dBVrms/div).
14) Math operations on waveforms: add, subtract, multiply.
15) Vertical and Horizontal cursors for waveform measurement.
16) The USB FS connector (CN13) provides USB connectivity to the board. The board implements USB Mass Storage Device Class (custom written USB stack).
17) Screenshot of waveforms can be taken and stored in the on-board 16MB QSPI Flash (upto 50 screenshots) in BMP format. They can be accessed from a host PC through USB. Only read access is allowed from the host PC. A FAT file system (FatFs library: http://elm-chan.org/fsw/ff/00index_e.html) is used on the QSPI Flash.
18) Screenshots can be taken by pressing the on-board user button (Blue). The saved screenshots can be browsed by swiping left/right gestures. Down swipe 
deletes the screenshot and Up swipe exits the screenshot display mode.
19) Pressing the user button (Blue) when resetting the board reformats the QSPI flash and all the stored screenshots will be lost.
‎

Two versions are provided:

* H7oscillo-v1: H7oscillo basic version
Basic version of the H7oscillo oscilloscope, with measurements, triggering, selectable display modes, FFT, math operations, dV & dT cursors, and captured waveform zoom-in/zoom-out features.

* H7oscillo-v2: H7oscillo full-featured version
In addition to the features of the previous release, added screenshot capture functionality and provided USB connectivity to the board to access the stored screenshots from host PC.
