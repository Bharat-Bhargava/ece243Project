#include "custom_defines.h"

volatile int ps2_data = 0;
volatile int ps2_flag = 0;
volatile int paused = 0;  // 0 = running, 1 = paused