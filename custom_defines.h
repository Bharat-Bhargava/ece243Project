
#ifndef CUSTOM_DEFINES_H
#define CUSTOM_DEFINES_H

// Custom macros
#define uint32 unsigned int
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 256
#define F_CPU 100000000  // CPU frequencey = 100Mhz
#define ACTUAL_X 320
#define ACTUAL_Y 240
#define WHITE 0xFFFF
#define BLACK 0x0000

#define BAT_HEIGHT 16
#define BAT_WIDTH 16

#define TIMERSEC 100000000  // 1 second

// Gblobal Variables
extern volatile int ps2_data;  // Stores the last PS/2 data byte
extern volatile int ps2_flag;  // Flag to indicate new PS/2 data
extern volatile int paused;    // 0 = running, 1 = paused
extern int hard_mode;  // 0 = normal mode, 1 = hard mode

#endif