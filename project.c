#include <stdlib.h>
#include "custom_defines.h"
#include "device_structs.h"
#include "address_map_niosv.h"
#include "project_functions.h"

// Device declarations
struct timer_t *const timer = TIMER_BASE;  // Timer
struct PIT_t *const ledp = LEDR_BASE;  // PIT
struct PIT_t  *const buttonp = KEY_BASE;  // Button


int main(void){





}