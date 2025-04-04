# Platformer Game for DE1-SoC FPGA

This project implements a platformer game on the DE1-SoC FPGA using C. The game features a bat character that can jump between platforms, with gravity and collision mechanics. The project utilizes hardware-specific features like video output, PS2 input, and timers.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Setup and Compilation](#setup-and-compilation)
- [How to Play](#how-to-play)
- [Video Demo](#video-demo)

## Overview
The game is designed to run on the DE1-SoC FPGA platform. It uses hardware peripherals such as:
- **Video Output**: For rendering the game graphics. (done Via VGA)
- **PS2 Keyboard**: For player input.
- **Timer**: For managing game timing and delays.

The game logic includes platform generation, sprite rendering, and physics simulation (gravity and jumping).

## Features
- **Platformer Gameplay**: Jump between platforms while avoiding falling.
- **Sprite Rendering**: Dynamic sprite drawing and animation.
- **Keyboard Input**: Control the bat using a PS2 keyboard.(interupt based)- Control Hedwig's flight 
- **Hardware Acceleration**: Optimized for the DE1-SoC FPGA using the Nios V processor.

## Project Structure


## Dependencies
- **Intel FPGA Quartus Prime**: For programming the DE1-SoC FPGA.
- **C Compiler**: Compatible with the Nios V soft processor.
- **DE1-SoC Board**: Required to run the game.
- **GDB Setup with NIOS V** : more info can be found here: https://fpgacademy.org/tutorials.html 

## How to Play

- Connect a PS2 keyboard to the DE1-SoC FPGA.
- Use the spacebar to control Hedwig's flight.
- Navigate through obstacles without colliding.
- The game ends if Hedwig crashes into an obstacle.

## Video Demo
- Soon!