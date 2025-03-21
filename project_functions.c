
#include "project_functions.h"

#include "custom_defines.h"
#include "device_structs.h"
#include "screens.h"

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
    if((ps2->DATA == 0x74 || ps2->DATA == 0x6b)){
    while (ps2->DATA & 0x8000) {     // extract RVALID
      char byte = ps2->DATA & 0xFF;  // extract byte
      if(ps2->DATA != 0x40){         // ignore the make code of 0x4 before displaying
      ledp->DR = byte;
      } else{
        continue;
      }
    }
    }
  }
}




void keyboard2(struct PS2_t *const ps2, struct PIT_t *const ledp){
   ps2->DATA = 0xff;
   while(1){
    if(ps2->DATA == 0x1c || ps2->DATA == 0x23){
      while(ps2->DATA & 0x8000){
        char byte = ps2->DATA & 0xff;
        if(ps2->DATA != 0x40){
          ledp->DR = byte;
        } else {
          continue;
        }
      }
    }
   }
}