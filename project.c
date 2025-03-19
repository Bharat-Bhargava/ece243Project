#include <stdlib.h>

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"
#include "project_functions.h"
#include "sprites.h"

// Device declarations
struct timer_t *const timer = (int *)TIMER_BASE;           // Timer
struct PIT_t *const ledp = (int *)LEDR_BASE;               // PIT
struct PIT_t *const buttonp = (int *)KEY_BASE;             // Button
struct videoout_t *const vp = (int *)PIXEL_BUF_CTRL_BASE;  // Video

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

int main(void) {
  gameStart(vp->fbp, buttonp, ledp);
  clear_screen(vp->fbp);
  sprite_scroll(vp->fbp);  // ghost walking horizontally
}

//hello