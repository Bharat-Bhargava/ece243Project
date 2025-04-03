
#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"

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

void waitasec2(float seconds, struct timer_t *timer);  // improved game delay

// Main game loop clean up helper functions
void handle_input(int *move_left, int *move_right);
void update_movement(int *bat_x, int *bat_y, int *velocity_y, int move_left,
                     int move_right, int gravity, int jump_strength,
                     int ground_y, int screen_width);
void draw_sprite(struct videoout_t *vp, int frame, int bat_x, int bat_y,
                 int prev_bat_x, int prev_bat_y);

// Platform properties
#define PLATFORM_WIDTH 50  // Width of each platform
#define PLATFORM_HEIGHT 5  // Height of each platform
#define MAX_PLATFORMS 5    // Maximum number of platforms

// Define the Platform struct
typedef struct {
  int x;           // X position of the platform
  int y;           // Y position of the platform
  int width;       // Width of the platform
  int height;      // Height of the platform
  int prev_x;      // Previous X position (for erasing)
  int prev_y;      // Previous Y position (for erasing)
  int is_red;      // 1 if the platform is red, 0 otherwise
  int is_blue;     // 1 if the platform is blue, 0 otherwise
  int velocity_x;  // Horizontal velocity for moving platforms
} Platform;

void init_platforms(Platform platforms[], int screen_width, int screen_height);
void draw_platforms(struct fb_t *fbp, Platform platforms[], int erase);
void update_platforms(Platform platforms[], int bat_y, int screen_width,
                      int screen_height);

void check_collision(Platform platforms[], int *bat_x, int *bat_y,
                     int *velocity_y, int jump_strength);

void update_physics(int *bat_x, int *bat_y, int *velocity_y, int move_left,
                    int move_right, int gravity, int jump_strength,
                    int screen_width);

void batAudio(int *samples, int numOfSamples);
void gameOver();

#endif