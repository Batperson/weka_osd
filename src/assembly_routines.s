/*
 * assembly_routines.c
 * Optimized memory manipulation
 * Created on: 04/10/2018
 */

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb


  .section  .text.memclr
  .type  memclr, %function
memclr:

FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

  /* return */
  bx  lr
.size  memclr, .-memclr

