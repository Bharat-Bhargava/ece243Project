#include "custom_defines.h"

volatile int ps2_data = 0;
volatile int ps2_flag = 0;
volatile int paused = 0;  // 0 = running, 1 = paused

int hard_mode = 0;  // 0 = normal mode, 1 = hard mode

int highest_y = 240;     // Track the highest position (lowest y value)
int total_score = 0;     // Initialize the score to 0
int total_distance = 0;  // Total upward distance climbed



 // Key state tracking
 int move_left = 0;
 int move_right = 0;