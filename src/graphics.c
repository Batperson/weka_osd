/*
 * graphics.c
 *
 */

#include "stm32f4xx.h"
#include "graphics.h"

PIXEL  linebuf0[1024] __attribute__((aligned(1024)));
PIXEL  linebuf1[1024] __attribute__((aligned(1024)));
