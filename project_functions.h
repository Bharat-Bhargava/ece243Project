
#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"

// Function prototypes
/*
void initialize_timer(struct timer_t *timer, uint32 period);
uint32 read_switches(void);
uint32 read_buttons(void);
void set_leds(uint32 value);

*/
void plot_pixel(int x, int y, short int line_color, struct fb_t *const fbp);
void clear_screen(struct fb_t *const fbp);
int abs(int num);
void draw_line(int x0, int y0, int x1, int y1, short color,
               struct fb_t *const fbp);
void fbswap(struct videoout_t *const vp);

void gameStart(struct fb_t *const fbp, struct PIT_t *buttonp,
               struct PIT_t *ledp);

void waitasec(int pow_fraction, struct timer_t *time);

void sprite_draw(struct fb_t *const fbp, unsigned short sprite[], int x, int y);

void draw_screen(struct fb_t *const fbp, const unsigned short *screen);

void keyboard(struct PS2_t *const ps2, struct PIT_t *const ledp);

#endif