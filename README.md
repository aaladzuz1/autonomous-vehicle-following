# Autonomous Vehicle Following Another Vehicle (PIC + IR Sensors)

This repository contains an embedded systems project: an autonomous vehicle that follows another vehicle and maintains distance using infrared distance sensors and PWM motor control. The project was developed as a seminar project for the Faculty of Electrical Engineering, University of Sarajevo (September 2023).

## Project Overview
The system uses two IR distance sensors to measure the relative position of the vehicle ahead, converts sensor voltages via ADC, computes distance, and adjusts motor speed/direction to follow the target vehicle autonomously.

Key goal: implement autonomous movement (decision-making without human control) using sensors + control logic on a microcontroller.

## Hardware
- Microcontroller: Microchip PIC16F1939 (PIC16 family, 8-bit RISC)
- Distance sensors: 2× Sharp 2Y0A02 infrared distance sensors (analog output)
- Motor driver: L298N (PWM speed control + direction control via H-bridge)
- Motors: 2× IG32 24VDC 74 RPM DC motors
- Design artifacts: Proteus schematic + PCB design (2D + 3D views)

## Control Logic
1. Read both sensor voltages via ADC.
2. Convert voltage to distance using fitted sensor characteristics.
3. Compare left vs right distance:
   - If the right sensor indicates the target is further than the left (beyond a threshold), turn left.
   - If the right sensor indicates the target is closer than the left (beyond a threshold), turn right.
   - Otherwise, drive straight and adjust speed based on distance from the desired following distance.

The code defines core parameters like `DESIRED_DISTANCE`, `DISTANCE_THRESHOLD`, and PWM limits.

## Software
- Language: C 
- Development environment: MPLAB X IDE (used for development and simulation before hardware testing)

## Repository Contents
- src/ – PIC C source code
- docs/ – project report / documentation
- proteus/ – Proteus schematic/project exports and pcb 2D/3D outputs
