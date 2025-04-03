# Platformer Game for DE1-SoC FPGA

This project implements a platformer game on the DE1-SoC FPGA using C. The game features a bat character that can jump between platforms, with gravity and collision mechanics. The project utilizes hardware-specific features like video output, PS2 input, and timers.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Setup and Compilation](#setup-and-compilation)
- [How to Play](#how-to-play)
- [Code Highlights](#code-highlights)
- [License](#license)

## Overview
The game is designed to run on the DE1-SoC FPGA platform. It uses hardware peripherals such as:
- **Video Output**: For rendering the game graphics.
- **PS2 Keyboard**: For player input.
- **Timer**: For managing game timing and delays.

The game logic includes platform generation, sprite rendering, and physics simulation (gravity and jumping).

## Features
- **Platformer Gameplay**: Jump between platforms while avoiding falling.
- **Sprite Rendering**: Dynamic sprite drawing and animation.
- **Keyboard Input**: Control the bat using a PS2 keyboard.
- **Hardware Acceleration**: Utilizes FPGA-specific hardware for performance.

## Project Structure
