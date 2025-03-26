
#include "project_functions.h"

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"
#include "screens.h"
#include "sprites.h"

// void initialize_timer(struct timer_t *timer, uint32 period) {
//   timer->control = 0;  // Disable timer
//   timer->periodlo = period & 0xFFFF;
//   timer->periodhi = (period >> 16) & 0xFFFF;
//   timer->control = 1;  // Enable timer
// }

// void set_leds(uint32 value) { *(volatile uint32 *)LEDR_BASE = value; }

// uint32 read_switches(void) { return *(volatile uint32 *)SW_BASE; }

// uint32 read_buttons(void) { return *(volatile uint32 *)KEY_BASE; }

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
  buttonp->EDGE = 0xF;  // clear edge bits
  draw_screen(fbp, start_screen);
  while (1) {
    ledp->DR = buttonp->DR;
    if (buttonp->EDGE & 0x1) {
      break;
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
  for (int i = 0; i < MAX_PLATFORMS; i++) {
    platforms[i].x =
        rand() % (screen_width - PLATFORM_WIDTH);  // Random x position
    platforms[i].y = screen_height - (i * 50);     // Space platforms vertically
    platforms[i].width = PLATFORM_WIDTH;           // Platform width
    platforms[i].height = PLATFORM_HEIGHT;         // Platform height
    platforms[i].prev_x = platforms[i].x;          // Initialize previous x
    platforms[i].prev_y = platforms[i].y;          // Initialize previous y
  }
}

// Draw or erase platforms
void draw_platforms(struct fb_t *fbp, Platform platforms[], int erase) {
  unsigned short color =
      erase ? WHITE : BLACK;  // Use WHITE to erase, BLACK to draw

  for (int i = 0; i < MAX_PLATFORMS; i++) {
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
    platforms[i].y +=
        (240 / 2 - bat_y) / 10;  // Move platforms up as the bat moves higher

    // Recycle platforms that move off the screen
    if (platforms[i].y > screen_height) {
      platforms[i].x =
          rand() % (screen_width - PLATFORM_WIDTH);  // New random x position
      platforms[i].y = -PLATFORM_HEIGHT;             // Place above the screen
    }
  }
}

#define BAT_WIDTH 15
#define BAT_HEIGHT 16

void check_collision(Platform platforms[], int *bat_x, int *bat_y,
                     int *velocity_y, int jump_strength) {
  for (int i = 0; i < MAX_PLATFORMS; i++) {
    // Check if the bat and platform rectangles intersect
    if (*bat_x < platforms[i].x + platforms[i].width &&
        *bat_x + BAT_WIDTH > platforms[i].x &&
        *bat_y < platforms[i].y + platforms[i].height &&
        *bat_y + BAT_HEIGHT > platforms[i].y &&
        *velocity_y > 0) {
      // Snap the bat to the top of the platform
      *bat_y = platforms[i].y - BAT_HEIGHT;
      *velocity_y = jump_strength;
      return;
    }
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


//Score update code below and display on HEX
volatile unsigned int *hex3_hex0_ptr = (volatile unsigned int *) HEX3_HEX0_BASE;
volatile unsigned int *hex5_hex4_ptr = (volatile unsigned int *) HEX5_HEX4_BASE;

// Seven-segment display encoding for digits 0-9 (adjust if needed)
const unsigned char hex_segs[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};


int highest_y = 20;    // Track the highest position (lowest y value)
void score(int currentBat_y){
int score = 0;

   // If the bat is higher than ever before (smaller y-value)
    if (currentBat_y > highest_y) {
        // Increase score by the difference in height
        score += highest_y + currentBat_y;    //score = score + highestY + currentBatY
        highest_y = currentBat_y;
    }
}

void display_score(int score) {
    unsigned char digits[6];
    int i;
    
    // Extract 6 digits from the score (least significant first).
    for (i = 0; i < 6; i++) {
        digits[i] = score % 10;
        score /= 10;
    }
    
    // Pack HEX5 and HEX4: HEX5 gets digits[5] and HEX4 gets digits[4].
    unsigned int hex5_hex4_val = (hex_segs[digits[5]] << 8) | hex_segs[digits[4]];
    
    // Pack HEX3, HEX2, HEX1, HEX0.
    unsigned int hex3_hex0_val = (hex_segs[digits[3]] << 24) |
                             (hex_segs[digits[2]] << 16) |
                             (hex_segs[digits[1]] << 8)  |
                             (hex_segs[digits[0]]);
    
    // Write the packed values to the HEX display registers.
    *hex5_hex4_ptr = hex5_hex4_val;
    *hex3_hex0_ptr = hex3_hex0_val;
}


