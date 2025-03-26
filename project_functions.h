
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

void keyboard2(struct PS2_t *const ps2, struct PIT_t *const ledp);

void sprite_draw2(struct fb_t *const fbp, unsigned short sprite[], int x, int y,
    int prev_x, int prev_y);

void draw_pause(void);


void waitasec2(float seconds, struct timer_t *timer); // improved game delay


//Main game loop clean up helper functions
void handle_input(int *move_left, int *move_right);
void update_movement(int *bat_x, int *bat_y, int *velocity_y, int move_left, int move_right, int gravity, int jump_strength, int ground_y, int screen_width);
void draw_sprite(struct videoout_t *vp, int frame, int bat_x, int bat_y, int prev_bat_x, int prev_bat_y);


#endif