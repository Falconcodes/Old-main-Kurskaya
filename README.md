STM-based devices
=====
Here you can find some useful libraries for STM32F0 microcontroller.

# Hardware

Board: STM32F0Discovery

Microcontroller: STM32F051R8T6

# Contents
This repository based on
* Standard IAR ARM Workspace.
* STM32F0Discovery Board Peripherals library.

Peripheral libraries:
* LCD1602 library (write text on character LCD display).
* NIXIE library (write digits on old neon indicator lamps).
* nrf24L01+ library (RF-transmitter).
* PCF8583 library (Real Time Clock).
* DHT11 library (Humidity & Temperature Sensor).

Modules:
* USART module (write debug information).
* ADC DMA module (for simultaneous reading up to 8 channels).
* Encoder module (use internal Timer to read value from encoders).
* Servo module (use internal Timer, variable duty cycle, 50 Hz).