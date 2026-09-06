# STM32-ARM-Sensor-Monitoring-System
ARM-based sensor monitoring and control system using STM32L031K6, ADC, I2C LCD, UART, GPIO, and interrupts.
# STM32 ARM-Based Sensor Monitoring & Control System

An ARM-based embedded monitoring and control system developed using the STM32L031K6 microcontroller and simulated in Wokwi.

## Overview

The system acquires an analog sensor signal using the STM32 ADC, converts the sensor reading into a percentage, classifies the system condition into three states, and provides visual and LCD-based status information.

## System States

| Sensor Level | System State | LED |
|---|---|---|
| 0–30% | NORMAL | Green |
| 31–70% | WARNING | Yellow |
| 71–100% | ALERT | Red |

## Hardware

- STM32L031K6 Nucleo board
- Potentiometer as analog sensor
- 16x2 I2C LCD
- Green LED
- Yellow LED
- Red LED
- Push button
- 220Ω resistors

## Interfaces

- ADC
- GPIO
- I2C
- UART
- External Interrupt

## Pin Configuration

| Component | STM32 Pin |
|---|---|
| Analog Sensor | A0 |
| Push Button | A1 |
| LCD SDA | D4 |
| LCD SCL | D5 |
| Green LED | D3 |
| Yellow LED | D6 |
| Red LED | D12 |

## Software

- Embedded C/C++
- Arduino STM32 Core
- Wokwi
- Serial Monitor

## Features

- 12-bit ADC sensor acquisition
- Real-time sensor processing
- Three-state decision logic
- LED status indication
- I2C LCD monitoring
- UART serial monitoring
- Push-button interrupt handling
- Embedded system testing and debugging

## Results

The system was successfully tested across normal, warning, and alert operating conditions.

Example outputs:

```text
ADC = 1137 | Level = 27% | Status = NORMAL
ADC = 2610 | Level = 63% | Status = WARNING
ADC = 3839 | Level = 93% | Status = ALERT
