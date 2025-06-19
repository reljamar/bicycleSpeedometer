# BICYCLE SPEEDOMETER PROJECT
*A simple LCD bicycle speedometer. This project is a work in progress.*

## Purpose
This project is an exercise in embedded systems, and as an excuse to use 7-segment LCDs. With some refinement, it can be turned into a useable product. This is a home project, not optimized for mass production, but rather for hand-soldering, simple handling, and parts availability. The housing can be 3D printed.

## Description
A custom low-power digital speedometer for a bicycle, built around an STM32 microcontroller with integrated LCD driver. This project emphasizes hardware efficiency, clear UI, and compact design — using a reed switch instead of a hall sensor for speed sensing, and a low-power LCD segment display. It is still under development, with core functionality working and additional features planned.

## Overview
- Displays real-time speed on a 3-digit 7-segment LCD with one decimal precision
- Reed switch and magnet used for wheel rotation sensing
- Custom PCB with STM32L053R6T6, featuring onboard LCD driving and USB debug interface
- 350 mAh Li-Po battery with integrated charging via USB
- Buttons for scrolling through displays (current speed, average speed since reset, distance since reset, driving time since reset, odometer)
- Designed for ultra-low power operation (with further optimization planned)

## Project status
Currently the project can achieve the core function of the speedometer: measure the speed, and display it on the display, as well as charge the battery.
<p align="center">
<img src="images/project-current-status.jpg" alt="Current build" width="400"/>
</p>
The magnet and sensor are on the bicycle which couldn't fit in this picture. Top cover is also not shown.

## System architecture
### Microcontroller
The system is based on a STM32L053R6T6 microcontroller. The MCU has an LCD driver built-in, and is low-power. The CPU is a Cortex-M0, which is plenty for this application. The MCU reads the reed switch status, calculates the speed, displays it on the LCD, monitors the battery, stores data.
### Display
The display on this device is made of two parts: the 7-segment LCD, and indication diodes. The diodes show the meaning of the current information on the display, and the units. While the LCD power consumption is very low, the diodes are not an optimal choice for low power. It is possible to order custom LCDs for a reasonable price, so a future addition to the project will be to replace the indication diodes to marked LCD segments.
### Sensor
The sensor is activated by a magnet attached to the wheel spokes. The sensor is a simple reed switch. This is done to minimize current draw. A Hall effect sensor or Hall effect switch constantly consumes a few mA, whereas the reed switch only draws current during the small percantage of the rotation period when it is closed by the magnet on the wheel, and that current is limited by a pull-up resistor. Due to the nature of using a mechanical switch, it is prone to bouncing. That is why an RC filter is implemented, as well as debounce software. The magnet is strong enough to keep the switch closed for a significant enough portion of the period to allow time for the bouncing to settle.
<p align="center">
<img src="images/magnet-distance.jpeg" alt="Current build" width="400"/>
</p>
