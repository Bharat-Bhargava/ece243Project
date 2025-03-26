#include "address_map_niosv.h"
#include "custom_defines.h"
#include "device_structs.h"

// void set_PS2_interrput(void){
//     volatile int *PS2_ptr = (int *)PS2_BASE;
//     *(PS2_ptr + 1) = 0x1; // enable interrupts
//     *(PS2_ptr) = 0xFF; // reset
// }

// void set_PS2_interrput(void) {
//   struct PS2_t *const ps2 = (int *)PS2_BASE;
//   ps2->CONTROL = 0x1;  // Enable interrupts (set RE bit)
//   ps2->DATA = 0xFF;    // Reset the PS/2 device
// }
void set_PS2_interrput(void) {
  struct PS2_t *const ps2 = (struct PS2_t *)PS2_BASE;

  ps2->CONTROL = 0x1;  // Enable interrupts (set RE bit)

  // Send a reset command to the PS/2 device
  // ps2->DATA = 0xFF;  // Reset command

  // Read the acknowledgment byte (should be 0xFA for ACK)
  // volatile int ack = ps2->DATA & 0xFF;

  // Clear the FIFO by reading all available data only if RAVAIL > 0
  while ((ps2->DATA >> 16) & 0xFF) {        // Check RAVAIL (bits 31:16)
    volatile int dummy = ps2->DATA & 0xFF;  // Read and discard data
  }
}
// void PS2_ISR(void) {
//   struct PS2_t *const ps2 = (int *)PS2_BASE;

//   // ps2_data = ps2->DATA & 0xFF;  // Extract the data byte
//   // ps2_flag = 1;                 // Set the flag
//   // ps2->CONTROL = 0x1;           // Acknowledge interrupt

//   // Check if RVALID is set (data is available)

//   ps2_data = ps2->DATA & 0xFF;  // Extract the data byte
//   ps2_flag = 1;                 // Set the flag to indicate new data

//   // Acknowledge the interrupt by clearing the FIFO
//   // ps2->CONTROL = 0x1;  // Re-en
// }

// void PS2_ISR(void) {
//   struct PS2_t *const ps2 = (int *)PS2_BASE;
//   static int isBreak = 0;  // Flag to track break code sequence

//   // Fully clear the FIFO by reading all available data
//   while ((ps2->DATA >> 16) & 0xFF) {  // Check RAVAIL (bits 31:16)
//     int data = ps2->DATA & 0xFF;      // Extract the data byte

//     if (data == 0xF0) {  // Break code detected
//       isBreak = 1;       // Set the break flag
//       continue;          // Ignore the break code itself
//     }

//     if (isBreak) {
//       isBreak = 0;  // Reset the break flag
//       continue;     // Ignore key release events
//     }

//     // Process valid make codes (key press events)
//     ps2_data = data;  // Store the valid data byte
//     ps2_flag = 1;     // Set the flag to indicate new data
//   }

//   // Acknowledge the interrupt by re-enabling interrupts
//   ps2->CONTROL = 0x1;  // Set RE bit to re-enable interrupts
// }

void PS2_ISR(void) {
  struct PS2_t *const ps2 = (struct PS2_t *)PS2_BASE;
  static int isBreak = 0;  // Flag to track break code sequence

  // Fully clear the FIFO by reading all available data
  while (1) {
    int ps2_data_register = ps2->DATA;  // Read ps2->DATA once
    int ravail =
        (ps2_data_register >> 16) & 0xFF;  // Extract RAVAIL (bits 31:16)

    if (ravail == 0) {
      break;  // Exit the loop if FIFO is empty
    }

    int data = ps2_data_register & 0xFF;  // Extract the data byte (bits 7:0)

    if (data == 0xF0) {  // Break code detected
      isBreak = 1;       // Set the break flag
      continue;          // Ignore the break code itself
    }

    if (isBreak) {
      // Key release: pass the released key to the main loop
      ps2_data = data | 0x80;  // Mark the key as released (add 0x80)
      ps2_flag = 1;            // Set the flag to indicate new data
      isBreak = 0;             // Reset the break flag
      continue;
    }

    // Handle the pause key (e.g., 0x76 for the Pause key)
    if (data == 0x76) {  // Pause key pressed
      paused = !paused;  // Toggle the paused state
      continue;          // Skip further processing for the pause key
    }

    // Process valid make codes (key press events)
    if (data != 0xF0) {  // Ignore stray break codes
      ps2_data = data;   // Store the valid data byte
      ps2_flag = 1;      // Set the flag to indicate new data
    }
  }

  // Acknowledge the interrupt by re-enabling interrupts
  ps2->CONTROL = 0x1;  // Set RE bit to re-enable interrupts
  // Re-enable global interrupts (set MIE bit in mstatus)
  __asm__ volatile("csrs mstatus, %0" ::"r"(0b1000));
}

void handler(void) {
  int mcause_value;
  __asm__ volatile("csrr %0, mcause" : "=r"(mcause_value));

  if (mcause_value == 0x80000016) {  // Check if IRQ 22 (PS/2)
    PS2_ISR();
  } else {
    *(volatile int *)LEDR_BASE = 0xFF;  // Turn on all LEDs
  }
}

void processor_side_setup(void) {
  int mtvec_value = (int)&handler;
  __asm__ volatile("csrw mtvec, %0" ::"r"(mtvec_value));

  int mstatus_value = 0b1000;  // Enable MIE
  __asm__ volatile("csrs mstatus, %0" ::"r"(mstatus_value));

  int mie_value = 1 << 22;  // Enable IRQ 22
  __asm__ volatile("csrs mie, %0" ::"r"(mie_value));
}