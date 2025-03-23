#include <stdlib.h>

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"
#include "interupt.h"
#include "project_functions.h"
#include "sprites.h"

// Device declarations
struct timer_t *const timer = (int *)TIMER_BASE;           // Timer
struct PIT_t *const ledp = (int *)LEDR_BASE;               // PIT
struct PIT_t *const buttonp = (int *)KEY_BASE;             // Button
struct videoout_t *const vp = (int *)PIXEL_BUF_CTRL_BASE;  // Video
struct PS2_t *const ps2 = (int *)PS2_BASE;                 // PS2

void sprite_scroll(struct fb_t *const fbp) {
  int x, y, s, ss;

  s = 0;
  ss = 0;
  for (x = 0; x < ACTUAL_X; x += 1) {
    sprite_draw(fbp, ghost[s], x, 120);
    sprite_draw(fbp, bat[s], ACTUAL_X - 16 - x, 20);
    sprite_draw(fbp, frog[s], x, 60);
    sprite_draw(fbp, skeleton[ss], ACTUAL_X - 16 - x, 80);

    s = (s + 1) % 2;
    ss = (ss + 1) % 3;
    waitasec(2, timer);
  }
}

// int main(void) {
//  // keyboard2(ps2, ledp);
//   gameStart(vp->fbp, buttonp, ledp);
//   clear_screen(vp->fbp);
//   sprite_scroll(vp->fbp);  // ghost walking horizontally
// }

// hello

int mainer(void) {
  gameStart(vp->fbp, buttonp, ledp);
  int bat_x = 160;                             // Initial x position of the bat
  int bat_y = 20;                              // Initial y position of the bat
  int prev_bat_x = bat_x, prev_bat_y = bat_y;  // Previous position of the bat
  int frame = 0;                               // Current frame for animation
  int isBreak = 0;  // Flag to indicate if the current code is a break code
  int speed = 5;    // Step size to move the sprite faster

  clear_screen(vp->fbp);

  while (1) {
    if (ps2->DATA & 0x8000) {        // Check RVALID
      char byte = ps2->DATA & 0xFF;  // Extract the byte

      // If the byte is 0xF0, it's the start of a break code sequence
      if (byte == 0xF0) {
        isBreak = 1;  // Set the break code flag
        continue;
      }

      if (!isBreak) {  // Process only make codes
        switch (byte) {
          case 0x1D:  // 'W' key for moving up
            if (bat_y > 0) {
              bat_y -= speed;
            }
            break;
          case 0x1C:  // 'A' key for moving left
            if (bat_x > 0) {
              bat_x -= speed;
            }
            break;
          case 0x23:  // 'D' key for moving right
            if (bat_x < ACTUAL_X - 16) {
              bat_x += speed;
            }
            break;
          case 0x1B:  // 'S' key for moving down
            if (bat_y < ACTUAL_Y - 16) {
              bat_y += speed;
            }
            break;
          default:
            break;  // Ignore other keys
        }
      } else {
        isBreak = 0;  // Reset the break code flag after processing
      }
    }

    // Clear the previous position of the bat
    sprite_draw(vp->bfbp, (unsigned short[]){WHITE}, prev_bat_x, prev_bat_y);

    // Draw the current frame of the bat
    sprite_draw(vp->bfbp, bat[frame], bat_x, bat_y);

    // Swap buffers
    fbswap(vp);

    // Update the frame for animation
    frame = (frame + 1) % 3;

    // Store the current position as the previous position
    prev_bat_x = bat_x;
    prev_bat_y = bat_y;

    // Add a small delay
    // waitasec(1, timer);
  }
}

int main(void) {
  // Processor-side setup for interrupts
  processor_side_setup();

  // Device-side setup for PS/2 interrupts
  set_PS2_interrput();

  // Main loop
  while (1) {
    if (ps2_flag) {  // Check if new PS/2 data is available
      ps2_flag = 0;  // Reset the flag

      // Display the PS/2 data on the LEDs
      *(volatile int *)LEDR_BASE = ps2_data;
    }
  }

  // gameStart(vp->fbp, buttonp, ledp);
}