
#include "project_functions.h"

void initialize_timer(struct timer_t *timer, uint32 period) {
  timer->control = 0;  // Disable timer
  timer->periodlo = period & 0xFFFF;
  timer->periodhi = (period >> 16) & 0xFFFF;
  timer->control = 1;  // Enable timer
}

void set_leds(uint32 value) { *(volatile uint32 *)LEDR_BASE = value; }

uint32 read_switches(void) { return *(volatile uint32 *)SW_BASE; }

uint32 read_buttons(void) { return *(volatile uint32 *)KEY_BASE; }
