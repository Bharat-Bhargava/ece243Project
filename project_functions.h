
#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"

// Function prototypes
// Function prototypes
void initialize_timer(struct timer_t *timer, uint32 period);
uint32 read_switches(void);
uint32 read_buttons(void);

void clear_screen(struct fb_t *const fbp, unsigned short color);

void move_bat(struct fb_t *const fbp, unsigned short *bat[], int *bat_x,
              int *bat_y, int *frame, uint32 key_state);
void sprite_draw(struct fb_t *const fbp, unsigned short sprite[], int x, int y);

#endif