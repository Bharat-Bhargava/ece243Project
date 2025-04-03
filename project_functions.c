#include "project_functions.h"

#include "address_map_niosv.h"
#include "audio.h"
#include "custom_defines.h"
#include "device_structs.h"
#include "screens.h"
#include "sprites.h"

int xn = ACTUAL_X;
int yn = ACTUAL_Y;

void plot_pixel(int x, int y, short int line_color, struct fb_t *const fbp) {
  fbp->pixels[y][x] = line_color;
}

void clear_screen(struct fb_t *const fbp) {
  for (int y = 0; y < ACTUAL_Y; y++) {
    for (int x = 0; x < ACTUAL_X; x++) {
      fbp->pixels[y][x] = WHITE;
    }
  }
}

void swap(int *px, int *py) {
  int tmp = *px;  // tmp <- px
  *px = *py;      // px <- py aka *px = y
  *py = tmp;      // py <- tmp
}

int abs(int num) {
  if (num >= 0) {
    return num;
  } else {
    return (-1) * (num);
  }
}

void draw_line(int x0, int y0, int x1, int y1, short color,
               struct fb_t *const fbp) {
  // Bresenham's algorithm

  // Determine if the line is steep (more vertical than horizontal)
  int is_steep = abs(y1 - y0) > abs(x1 - x0);

  // If the line is steep, swap the x and y coordinates
  if (is_steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  // Ensure that we draw the line from left to right
  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int deltax = x1 - x0;       // Compute the difference in x
  int deltay = abs(y1 - y0);  // Compute the absolute difference in y
  int error = -(deltax / 2);  // Initialize the error term (decision variable)

  int y_step = (y0 < y1) ? 1 : -1;  // Determine the y step direction
  int y = y0;                       // Start y at the initial y-coordinate

  // Loop through each x-coordinate from x0 to x1
  for (int x = x0; x <= x1; ++x) {
    if (is_steep) {
      plot_pixel(y, x, color,
                 fbp);  // Plot pixel with swapped coordinates if line is steep
    } else {
      plot_pixel(x, y, color,
                 fbp);  // Otherwise, plot pixel with original coordinates
    }
    error += deltay;  // Update the error term
    if (error > 0) {
      y += y_step;      // Move to the next y-coordinate if the error term is
                        // positive
      error -= deltax;  // Adjust the error term again
    }
  }
}

void fbswap(struct videoout_t *const vp) {
  vp->fbp = 1;  // request swap
  // wait for current displayed frame to finish
  while ((vp->StatusControl & 1) != 0);
}

// timer Delay
void waitasec(int pow_fraction, struct timer_t *timer) {
  unsigned int t = TIMERSEC >> pow_fraction;
  timer->control = 0x8;  // stop the timer
  timer->status = 0;     // reset TO
  timer->periodlo = (t & 0x0000FFFF);
  timer->periodhi = (t & 0xFFFF0000) >> 16;
  timer->control = 0x4;
  while ((timer->status & 0x1) == 0);
  timer->status = 0;  // reset TO
}

void waitasec2(float seconds, struct timer_t *timer) {
  unsigned int t =
      (unsigned int)(TIMERSEC * seconds);  // Calculate the precise delay
  timer->control = 0x8;                    // Stop the timer
  timer->status = 0;                       // Reset TO
  timer->periodlo = (t & 0x0000FFFF);
  timer->periodhi = (t & 0xFFFF0000) >> 16;
  timer->control = 0x4;                // Start the timer
  while ((timer->status & 0x1) == 0);  // Wait for the timer to expire
  timer->status = 0;                   // Reset TO
}

// Originally from Andreas Moshovos
void sprite_draw(struct fb_t *const fbp, unsigned short sprite[], int x,
                 int y) {
  int sxi, syi;
  int xi, yi;

  for (sxi = 0; sxi < 16; sxi++)
    for (syi = 0; syi < 16; syi++) {
      xi = x + sxi;
      yi = y + syi;
      fbp->pixels[yi][xi] = sprite[syi * 16 + sxi];
    }
}

// Edited version to inccorporate erasing
void sprite_draw2(struct fb_t *const fbp, unsigned short sprite[], int x, int y,
                  int prev_x, int prev_y) {
  int sxi, syi;
  int xi, yi;

  // Clear the previous position of the sprite
  for (sxi = 0; sxi < 16; sxi++) {
    for (syi = 0; syi < 16; syi++) {
      xi = prev_x + sxi;
      yi = prev_y + syi;
      fbp->pixels[yi][xi] = WHITE;  // Fill with background color
    }
  }

  // Draw the sprite at the new position
  for (sxi = 0; sxi < 16; sxi++) {
    for (syi = 0; syi < 16; syi++) {
      xi = x + sxi;
      yi = y + syi;
      fbp->pixels[yi][xi] = sprite[syi * 16 + sxi];
    }
  }
}

void gameStart(struct fb_t *const fbp, struct PIT_t *buttonp,
               struct PIT_t *ledp) {
  clear_screen(fbp);
  buttonp->EDGE = 0xF;  // Clear edge bits
  draw_screen(fbp, start_screen);

  while (1) {
    batAudio(mainScreen_samples, mainScreenNum_samples);
    if ((ps2_data == 0x16) || (ps2_data == 0x1E) || (ps2_data == 0x26)) {
      break;
    }
  }

  // Reset movement state
  move_left = 0;
  move_right = 0;

  while (1) {
    ledp->DR = buttonp->DR;
    if (ps2_data == 0x16) {  // Key 1 pressed
      hard_mode = 0;         // Enable hard mode
      break;
    } else if (ps2_data == 0x1E) {  // Key 2 pressed
      hard_mode = 1;                // Disable hard mode (normal mode)
      break;
    } else if (ps2_data == 0x26) {
      draw_screen(fbp, info_screen);
      while (1) {
        if (ps2_data == 0x21) {
          draw_screen(fbp, start_screen);
          break;
        }
      }

      continue;
    }
  }
  return;
}

void draw_screen(struct fb_t *const fbp, const unsigned short *screen) {
  for (int y = 0; y < ACTUAL_Y; y++) {
    for (int x = 0; x < ACTUAL_X; x++) {
      fbp->pixels[y][x] = screen[y * ACTUAL_X + x];
    }
  }
}

void keyboard(struct PS2_t *const ps2, struct PIT_t *const ledp) {
  ps2->DATA = 0xFF;  // reset

  while (1) {
    if ((ps2->DATA == 0x74 || ps2->DATA == 0x6b)) {
      while (ps2->DATA & 0x8000) {     // extract RVALID
        char byte = ps2->DATA & 0xFF;  // extract byte
        if (ps2->DATA !=
            0x40) {  // ignore the make code of 0x4 before displaying
          ledp->DR = byte;
        } else {
          continue;
        }
      }
    }
  }
}

void keyboard2(struct PS2_t *const ps2, struct PIT_t *const ledp) {
  ps2->DATA = 0xff;
  while (1) {
    if (ps2->DATA == 0x1c || ps2->DATA == 0x23) {
      while (ps2->DATA & 0x8000) {
        char byte = ps2->DATA & 0xff;
        if (ps2->DATA != 0x40) {
          ledp->DR = byte;
        } else {
          continue;
        }
      }
    }
  }
}

void draw_pause(void) {
  struct videoout_t *vp = (struct videoout_t *)PIXEL_BUF_CTRL_BASE;
  draw_screen(vp->bfbp, pause_screen);
  fbswap(vp);
}

// Input handling function
void handle_input(int *move_left, int *move_right) {
  if (ps2_flag) {  // Check if new PS/2 data is available
    ps2_flag = 0;  // Reset the flag

    if (ps2_data & 0x80) {                 // Key release
      int released_key = ps2_data & 0x7F;  // Extract the key code
      switch (released_key) {
        case 0x1C:  // 'A' key released
          *move_left = 0;
          break;
        case 0x23:  // 'D' key released
          *move_right = 0;
          break;
        default:
          break;
      }
    } else {  // Key press
      switch (ps2_data) {
        case 0x1C:  // 'A' key pressed
          *move_left = 1;
          *move_right = 0;  // Ensure mutual exclusivity
          break;
        case 0x23:  // 'D' key pressed
          *move_right = 1;
          *move_left = 0;  // Ensure mutual exclusivity
          break;
        default:
          break;
      }
    }
  }
}

// Movement update function
void update_movement(int *bat_x, int *bat_y, int *velocity_y, int move_left,
                     int move_right, int gravity, int jump_strength,
                     int ground_y, int screen_width) {
  // Apply movement based on key states
  if (move_left) {
    *bat_x -= 5;
  }
  if (move_right) {
    *bat_x += 5;
  }

  // Wrap around horizontally
  if (*bat_x < 0) {
    *bat_x = screen_width - 16;  // Wrap to the right edge
  } else if (*bat_x > screen_width - 16) {
    *bat_x = 0;  // Wrap to the left edge
  }

  // Apply gravity
  *velocity_y += gravity;

  // Update vertical position
  *bat_y += *velocity_y;

  // Check if the bat touches the ground
  if (*bat_y >= ground_y) {
    *bat_y = ground_y;            // Snap to the ground
    *velocity_y = jump_strength;  // Make the bat jump
  }
}

// Drawing function
void draw_sprite(struct videoout_t *vp, int frame, int bat_x, int bat_y,
                 int prev_bat_x, int prev_bat_y) {
  // Draw the sprite (clear previous position and draw at new position)
  sprite_draw2(vp->bfbp, bat[frame], bat_x, bat_y, prev_bat_x, prev_bat_y);

  // Swap buffers
  fbswap(vp);
}

#include <stdlib.h>  // For rand()

#include "project_functions.h"

// Initialize platforms
void init_platforms(Platform platforms[], int screen_width, int screen_height) {
  int platform_width = hard_mode
                           ? PLATFORM_WIDTH / 2
                           : PLATFORM_WIDTH;  // Smaller platforms in hard mode
  int platform_height = PLATFORM_HEIGHT;      // Height remains the same

  for (int i = 0; i < MAX_PLATFORMS; i++) {
    if (i == 0) {
      // Place the first platform at the bottom of the screen
      platforms[i].x =
          (screen_width - platform_width) / 2;  // Centered horizontally
      platforms[i].y = screen_height - platform_height;  // At the bottom
      platforms[i].is_red = 0;      // The first platform is always safe
      platforms[i].is_blue = 0;     // The first platform is not blue
      platforms[i].velocity_x = 0;  // No movement for the first platform
    } else {
      // Randomly position the other platforms
      platforms[i].x = rand() % (screen_width - platform_width);
      platforms[i].y = screen_height - (i * 50);  // Space platforms vertically

      // Assign red platforms with a 10% chance
      platforms[i].is_red = (rand() % 10 == 0);

      // Assign blue platforms with a 15% chance (but not red and blue at the
      // same time)
      platforms[i].is_blue = (!platforms[i].is_red && rand() % 7 == 0);

      // Set horizontal velocity for blue platforms
      platforms[i].velocity_x =
          platforms[i].is_blue ? (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1)
                               : 0;
    }
    platforms[i].width = platform_width;
    platforms[i].height = platform_height;
    platforms[i].prev_x = platforms[i].x;
    platforms[i].prev_y = platforms[i].y;
  }
}
// Draw or erase platforms
void draw_platforms(struct fb_t *fbp, Platform platforms[], int erase) {
  for (int i = 0; i < MAX_PLATFORMS; i++) {
    unsigned short color;
    if (erase) {
      color = WHITE;  // Erase with background color
    } else if (platforms[i].is_red) {
      color = 0xF800;  // RED for red platforms
    } else if (platforms[i].is_blue) {
      color = 0x001F;  // BLUE for blue platforms
    } else {
      color = BLACK;  // BLACK for normal platforms
    }

    for (int x = 0; x < platforms[i].width; x++) {
      for (int y = 0; y < platforms[i].height; y++) {
        int px = platforms[i].x + x;
        int py = platforms[i].y + y;

        // Ensure drawing stays within screen bounds
        if (px >= 0 && px < 320 && py >= 0 && py < 240) {
          fbp->pixels[py][px] = color;
        }
      }
    }

    // Update previous positions if drawing
    if (!erase) {
      platforms[i].prev_x = platforms[i].x;
      platforms[i].prev_y = platforms[i].y;
    }
  }
}
// Update platform positions
void update_platforms(Platform platforms[], int bat_y, int screen_width,
                      int screen_height) {
  for (int i = 0; i < MAX_PLATFORMS; i++) {
    // Move platforms up as the bat moves higher
    platforms[i].y += (240 / 2 - bat_y) / 10;

    // Move blue platforms left and right
    if (platforms[i].is_blue) {
      platforms[i].x += platforms[i].velocity_x;

      // Reverse direction if the platform hits the screen edges
      if (platforms[i].x <= 0 ||
          platforms[i].x + platforms[i].width >= screen_width) {
        platforms[i].velocity_x = -platforms[i].velocity_x;
      }
    }

    // Recycle platforms that move off the screen
    if (platforms[i].y > screen_height) {
      platforms[i].x = rand() % (screen_width -
                                 platforms[i].width);  // New random x position
      platforms[i].y = -platforms[i].height;           // Place above the screen
      platforms[i].is_red = (rand() % 10 == 0);        // Reassign red platforms
      platforms[i].is_blue =
          (!platforms[i].is_red && rand() % 7 == 0);  // Reassign blue platforms
      platforms[i].velocity_x =
          platforms[i].is_blue ? (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1)
                               : 0;
    }
  }
}

#define BAT_WIDTH 15
#define BAT_HEIGHT 16

void batAudio(int *samples, int numOfSamples) {
  struct audio_t *const audiop = (int *)AUDIO_BASE;
  int i;

  audiop->control = 0x8;  // clear the output FIFOs
  audiop->control = 0x0;  // resume input conversion
  for (i = 0; i < numOfSamples; i++) {
    // output data if there is space in the output FIFOs
    while (audiop->warc == 0);
    audiop->ldata = samples[i];
    audiop->rdata = samples[i];
  }
}

void update_physics(int *bat_x, int *bat_y, int *velocity_y, int move_left,
                    int move_right, int gravity, int jump_strength,
                    int screen_width) {
  // Apply horizontal movement
  if (move_left) {
    *bat_x -= 5;
  }
  if (move_right) {
    *bat_x += 5;
  }

  // Wrap around horizontally
  if (*bat_x < 0) {
    *bat_x = screen_width - 16;  // Wrap to the right edge
  } else if (*bat_x > screen_width - 16) {
    *bat_x = 0;  // Wrap to the left edge
  }

  // Apply gravity
  *velocity_y += gravity;

  // Cap the downward velocity (terminal velocity)
  if (*velocity_y > 10) {  // Adjust this value for smoother falling
    *velocity_y = 10;
  }

  // Update vertical position
  *bat_y += *velocity_y;
}

// Score update code below and display on HEX
volatile unsigned int *hex3_hex0_ptr = (volatile unsigned int *)HEX3_HEX0_BASE;
volatile unsigned int *hex5_hex4_ptr = (volatile unsigned int *)HEX5_HEX4_BASE;

// Seven-segment display encoding for digits 0-9 (adjust if needed)
const unsigned char hex_segs[10] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

void score(int currentBat_y, int prevBat_y) {
  // If the bat moves upward (currentBat_y < prevBat_y)
  if (currentBat_y < prevBat_y) {
    total_distance +=
        (prevBat_y - currentBat_y) / 10;  // Divide by 10 to slow down
  }
}
void display_score() {
  unsigned char digits[6] = {0};  // Array to store up to 6 digits
  int score = total_distance;     // Use the global total_distance variable
  int i;

  // Extract 6 digits from the score (least significant first)
  for (i = 0; i < 6; i++) {
    digits[i] = score % 10;  // Get the least significant digit
    score /= 10;             // Remove the least significant digit
  }

  // Pack HEX5 and HEX4: HEX5 gets digits[5] and HEX4 gets digits[4]
  unsigned int hex5_hex4_val = (hex_segs[digits[5]] << 8) | hex_segs[digits[4]];

  // Pack HEX3, HEX2, HEX1, HEX0
  unsigned int hex3_hex0_val = (hex_segs[digits[3]] << 24) |
                               (hex_segs[digits[2]] << 16) |
                               (hex_segs[digits[1]] << 8) | hex_segs[digits[0]];

  // Write the packed values to the HEX display registers
  *hex5_hex4_ptr = hex5_hex4_val;
  *hex3_hex0_ptr = hex3_hex0_val;
}

void check_collision(Platform platforms[], int *bat_x, int *bat_y,
                     int *velocity_y, int jump_strength) {
  struct PIT_t *buttonp = (struct PIT_t *)KEY_BASE;
  struct PIT_t *ledp = (struct PIT_t *)LEDR_BASE;
  struct videoout_t *vp = (struct videoout_t *)PIXEL_BUF_CTRL_BASE;

  for (int i = 0; i < MAX_PLATFORMS; i++) {
    // Check if the bat and platform rectangles intersect
    if (*bat_x < platforms[i].x + platforms[i].width &&
        *bat_x + BAT_WIDTH > platforms[i].x &&
        *bat_y + BAT_HEIGHT > platforms[i].y &&
        *bat_y < platforms[i].y + platforms[i].height) {
      if (platforms[i].is_red) {
        // Instant death on red platform
        waitasec(2,
                 (struct timer_t *)TIMER_BASE);  // Wait for 2 seconds (DELAY)
        gameOver();                              // Call game over function
        gameStart(vp->fbp, buttonp, ledp);       // Go back to the start screen
        *bat_x = 160;                            // Reset bat's position
        *bat_y = 100;
        *velocity_y = 0;                      // Reset velocity
        total_distance = 0;                   // Reset the score
        init_platforms(platforms, 320, 240);  // Reinitialize platforms
        clear_screen(vp->fbp);                // Clear the screen
        return;  // Exit after handling red platform
      }

      if (platforms[i].is_blue) {
        // Handle collision with blue platform
        if (*velocity_y > 0) {  // Only trigger collision if falling
          *bat_y = platforms[i].y -
                   BAT_HEIGHT;          // Snap bat to the top of the platform
          *velocity_y = jump_strength;  // Make the bat jump
          platforms[i].is_blue = 0;     // Mark as no longer blue
        }
      } else if (*velocity_y > 0) {
        // Snap the bat to the top of the platform
        *bat_y = platforms[i].y - BAT_HEIGHT;
        *velocity_y = jump_strength;  // Make the bat jump

        // Play jump sound effect
        batAudio(samples, samples_n);

        return;
      }
    }
  }
}

void gameOver() {
  struct PS2_t *const ps2 = (struct PS2_t *)PS2_BASE;
  ps2->CONTROL = 0x0;  // Disable interrupts (set RE bit)
  struct videoout_t *const vp = (int *)PIXEL_BUF_CTRL_BASE;  // Video
  draw_screen(vp->fbp, game_over_screen);

  waitasec(2, (struct timer_t *)TIMER_BASE);  // Wait for 2 seconds

  fbswap(vp);  // VSYNC
  batAudio(gameOverSamples, num_samples_gameOver);
  draw_screen(vp->bfbp, continue_screen);
  fbswap(vp);          // VSYNC
  ps2->CONTROL = 0x1;  // ReEnable interrupts (set RE bit)

  while (1) {
    if (ps2_data == 0x21) {
      return;
    }
  }
}
