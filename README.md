# Handheld 2D Platformer - Real-Time Embedded Gaming System

A custom-built, interrupt-driven handheld 2D platformer developed for the ARM Cortex-M0+ (MSPM0) microcontroller. This project features a custom graphics driver, hardware-level audio synthesis, and optimized physics engines.


## Features

* **Custom Graphics Driver:** Low-level ST7735 LCD driver implemented via SPI.
* **Interrupt-Driven Game Engine:** Uses Timer Interrupts to manage concurrent tasks including physics updates, sound generation, and display refreshes [cite: 62, 64].
* **Hardware Audio Synthesis:** Engineered a 5-bit R-2R DAC for real-time audio synthesis.
* **Analog Control:** Integrated ADC sampling to allow analog movement via joystick.

* **Custom Hardware:** PCB designed in KiCad, featuring hand-soldered buttons and resistors (not shown in video, switched from slide pot to joystick).

## Tech Stack

* **Languages:** C, ARM Assembly 
* **Hardware:** ARM Cortex-M0+ (MSPM0), ST7735 LCD 
* **Peripherals/Protocols:** SPI, UART, ADC/DAC
* **Tools:** KiCad (PCB Design) 


*Developed by Onik Hoque*
