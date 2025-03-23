#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"

// Function prototypes
void set_PS2_interrput(void);     // Device-side setup for PS/2 interrupts
void PS2_ISR(void);               // PS/2 interrupt service routine
void handler(void);               // Trap handler for interrupts
void processor_side_setup(void);  // Processor-side setup for interrupts

#endif  // INTERRUPT_H