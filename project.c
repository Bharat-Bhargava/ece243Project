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

    score(bat_y);

    // Add a small delay
    // waitasec(1, timer);
  }
}

// int main(void) {
//   // Processor-side setup for interrupts
//   processor_side_setup();
//   // Device-side setup for PS/2 interrupts
//   set_PS2_interrput();

//   // Main loop
//   while (1) {
//     if (ps2_flag) {  // Check if new PS/2 data is available
//       ps2_flag = 0;  // Reset the flag

//       // Display the PS/2 data on the LEDs
//       *(volatile int *)LEDR_BASE = ps2_data;
//     }
//   }

//   // gameStart(vp->fbp, buttonp, ledp);
// }

// int main(void) {
//   gameStart(vp->fbp, buttonp, ledp);
//   processor_side_setup();
//   set_PS2_interrput();

//   // Initialize variables
//   int bat_x = 160;  // Initial x position of the bat
//   int bat_y = 220;  // Initial y position of the bat (ground level)
//   int prev_bat_x = bat_x, prev_bat_y = bat_y;  // Previous position of the
//   bat int velocity_y = 0;                          // Vertical velocity const
//   int gravity = 1;                       // Gravity pulling the bat down
//   const int jump_strength = -15;  // Initial upward velocity when jumping
//   const int ground_y = 220;       // Y position of the ground
//   const int screen_width = 320;   // Width of the screen
//   int frame = 0;                  // Current frame for animation

//   clear_screen(vp->fbp);

//   while (1) {
//     if (paused) {
//       draw_pause();
//       while (paused);  // Wait until the game is unpaused
//       clear_screen(vp->fbp);
//       continue;
//     }

//     // Handle input (non-blocking)
//     if (ps2_flag) {  // Check if new PS/2 data is available
//       ps2_flag = 0;  // Reset the flag

//       switch (ps2_data) {
//         case 0x1C:     // 'A' key for moving left
//           bat_x -= 8;  // Move left
//           break;
//         case 0x23:     // 'D' key for moving right
//           bat_x += 8;  // Move right
//           break;
//         default:
//           break;  // Ignore other keys
//       }
//     }

//     // Wrap around horizontally
//     if (bat_x < 0) {
//       bat_x = screen_width - 16;  // Wrap to the right edge
//     } else if (bat_x > screen_width - 16) {
//       bat_x = 0;  // Wrap to the left edge
//     }

//     // Apply gravity
//     velocity_y += gravity;

//     // Update vertical position
//     bat_y += velocity_y;

//     // Check if the bat touches the ground
//     if (bat_y >= ground_y) {
//       bat_y = ground_y;            // Snap to the ground
//       velocity_y = jump_strength;  // Make the bat jump
//     }

//     // Draw the sprite (clear previous position and draw at new position)
//     sprite_draw2(vp->bfbp, bat[frame], bat_x, bat_y, prev_bat_x, prev_bat_y);

//     // Swap buffers
//     fbswap(vp);

//     // Update the frame for animation
//     frame = (frame + 1) % 3;

//     // Store the current position as the previous position
//     prev_bat_x = bat_x;
//     prev_bat_y = bat_y;

//     // Add a small delay to control the game speed
//     waitasec2(0.025, timer);
//   }

//   return 0;
// }

#include "device_structs.h"
#include "project_functions.h"
#include "sprites.h"

// int main(void) {
//   gameStart(vp->fbp, buttonp, ledp);
//   processor_side_setup();
//   set_PS2_interrput();

//   // Initialize variables
//   int bat_x = 160;  // Initial x position of the bat
//   int bat_y = 220;  // Initial y position of the bat (ground level)
//   int prev_bat_x = bat_x, prev_bat_y = bat_y;  // Previous position of the
//   bat int velocity_y = 0;                          // Vertical velocity const
//   int gravity = 1;                       // Gravity pulling the bat down
//   const int jump_strength = -15;  // Initial upward velocity when jumping
//   const int ground_y = 220;       // Y position of the ground
//   const int screen_width = 320;   // Width of the screen
//   int frame = 0;                  // Current frame for animation

//   // Key state tracking
//   int move_left = 0;   // Flag for moving left
//   int move_right = 0;  // Flag for moving right

//   clear_screen(vp->fbp);

//   while (1) {
//     if (paused) {
//       draw_pause();
//       while (paused);  // Wait until the game is unpaused
//       clear_screen(vp->fbp);
//       continue;
//     }

//     // Handle input
//     handle_input(&move_left, &move_right);

//     // Update movement
//     update_movement(&bat_x, &bat_y, &velocity_y, move_left, move_right,
//     gravity,
//                     jump_strength, ground_y, screen_width);

//     // Draw the sprite
//     draw_sprite(vp, frame, bat_x, bat_y, prev_bat_x, prev_bat_y);

//     // Update the frame for animation
//     frame = (frame + 1) % 3;

//     // Store the current position as the previous position
//     prev_bat_x = bat_x;
//     prev_bat_y = bat_y;

//     // Add a small delay to control the game speed
//     waitasec2(0.025, timer);
//   }

//   return 0;
// }

int main(void) {
  gameStart(vp->fbp, buttonp, ledp);
  processor_side_setup();
  set_PS2_interrput();

  // Initialize variables
  int bat_x = 160;                             // Initial x position of the bat
  int bat_y = 100;                             // Initial y position of the bat
  int prev_bat_x = bat_x, prev_bat_y = bat_y;  // Previous position of the bat
  int velocity_y = 0;                          // Vertical velocity
  const int gravity = 1;                       // Gravity pulling the bat down
  const int jump_strength = -15;  // Initial upward velocity when jumping
  const int screen_width = 320;   // Width of the screen
  const int screen_height = 240;  // Height of the screen
  int frame = 0;                  // Current frame for animation

  // Key state tracking
  int move_left = 0;
  int move_right = 0;

  // Platform data
  Platform platforms[MAX_PLATFORMS];

  // Initialize platforms
  init_platforms(platforms, screen_width, screen_height);

  clear_screen(vp->fbp);

  while (1) {
    if (paused) {
      draw_pause();
      while (paused);
      clear_screen(vp->fbp);
      continue;
    }

    // Handle input
    handle_input(&move_left, &move_right);

    // Update physics
    // Corrected function call
    update_physics(&bat_x, &bat_y, &velocity_y, move_left, move_right, gravity,
                   jump_strength, screen_width);

    // Check for platform collisions
    check_collision(platforms, &bat_x, &bat_y, &velocity_y, jump_strength);

    // Check if the bat falls below the screen
    if (bat_y > screen_height) {  // Only reset if the bat is below the screen
      gameStart(vp->fbp, buttonp, ledp);
      bat_x = 160;  // Reset bat's position
      bat_y = 100;
      velocity_y = 0;  // Reset velocity
      init_platforms(platforms, screen_width,
                     screen_height);  // Reinitialize platforms
      clear_screen(vp->fbp);
      continue;
    }

    // Update platforms
    draw_platforms(vp->bfbp, platforms, 1);  // Erase platforms
    update_platforms(platforms, bat_y, screen_width, screen_height);
    draw_platforms(vp->bfbp, platforms, 0);  // Draw platforms

    // Draw the sprite
    sprite_draw2(vp->bfbp, bat[frame], bat_x, bat_y, prev_bat_x, prev_bat_y);

    // Swap buffers
    fbswap(vp);

    // Update animation frame
    frame = (frame + 1) % 3;

    // Store previous position
    prev_bat_x = bat_x;
    prev_bat_y = bat_y;

    score(bat_y);

    // Add delay
    waitasec2(0.025, timer);
  }

  return 0;
}/// test