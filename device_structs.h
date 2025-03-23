#ifndef DEVICE_STRUCTS_H
#define DEVICE_STRUCTS_H

#include <stdlib.h>

#include "custom_defines.h"

struct fb_t {
  unsigned short volatile pixels[256][512];
};  // Frame Buffer Structure

struct videoout_t {
  struct fb_t *volatile fbp;   // front frame buffer
  struct fb_t *volatile bfbp;  // back frame buffer
  uint32 volatile resolution;  // resolution two fields of 16b each, packed into
                               // a 32b word
  uint32 volatile StatusControl;
};

struct timer_t {
  volatile uint32 status;
  volatile uint32 control;
  volatile uint32 periodlo;
  volatile uint32 periodhi;
  volatile uint32 snaplo;
  volatile uint32 snaphi;
};

struct PIT_t {
  volatile unsigned int DR;
  volatile unsigned int DIR;
  volatile unsigned int MASK;
  volatile unsigned int EDGE;
};

struct PS2_t {
  volatile unsigned int DATA;
  volatile unsigned int CONTROL;
};

struct audio_t {
  volatile unsigned int control;
  volatile unsigned char rarc;
  volatile unsigned char ralc;
  volatile unsigned char warc;
  volatile unsigned char walc;
  volatile unsigned int ldata;
  volatile unsigned int rdata;
};

#endif