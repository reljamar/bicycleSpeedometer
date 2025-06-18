# BICYCLE SPEEDOMETER PROJECT
*A simple LCD bicycle speedometer. This project is a work in progress.*

## Purpose
This project is an exercise in embedded systems, and as an excuse to use 7-segment LCDs. With some refinement, it can be turned into a useable product. This is a home project, not optimized for mass production, but rather for hand-soldering, simple handling, and parts availability. The housing can be 3D printed.

## Description
A custom low-power digital speedometer for a bicycle, built around an STM32 microcontroller with integrated LCD driver. This project emphasizes hardware efficiency, clear UI, and compact design — using a reed relay instead of a hall sensor for speed sensing, and a low-power LCD segment display. It is still under development, with core functionality working and additional features planned.

## Overview
- Displays real-time speed on a 3-digit 7-segment LCD with one decimal precision
- Reed relay and magnet used for wheel rotation sensing
- Custom PCB with STM32L073RZ, featuring onboard LCD driving and USB debug interface
- 350 mAh Li-Po battery with integrated charging via USB
- Buttons for scrolling through displays (current speed, average speed since reset, distance since reset, driving time since reset, odometer)
- Designed for ultra-low power operation (with further optimization planned)
