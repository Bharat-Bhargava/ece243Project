
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

#endif