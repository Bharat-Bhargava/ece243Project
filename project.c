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

extern total_distance;
int main(void) {
  processor_side_setup();
  set_PS2_interrput();

  gameStart(vp->fbp, buttonp, ledp);

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

  // Place the sprite on the first platform
  bat_x = platforms[0].x + (PLATFORM_WIDTH / 2) -
          (BAT_WIDTH / 2);              // Centered on the platform
  bat_y = platforms[0].y - BAT_HEIGHT;  // On top of the platform
  velocity_y = 0;                       // No movement initially

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

    // Update score based on upward movement
    score(bat_y, prev_bat_y);

    // Display the updated score on the HEX display
    display_score();

    // Check if the bat falls below the screen
    if (bat_y > screen_height) {  // Only reset if the bat is below the screen
      gameOver();
      gameStart(vp->fbp, buttonp, ledp);
      bat_x = 160;  // Reset bat's position
      bat_y = 100;
      velocity_y = 0;      // Reset velocity
      total_distance = 0;  // Reset the score
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

    // Add delay
    waitasec2(0.025, timer);
  }

  return 0;
}  /// test