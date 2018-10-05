/*
 * zerobuf.s
 * Routine for quickly zeroing a buffer, which must be word-aligned and a multiple of 48 bytes in size.
 * Created on: 04/10/2018
 */

.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb


.section .text
.global zerobuf
.type  zerobuf, %function	// r0: buffer start (word aligned), r1: buffer length (multiple of 48)

zerobuf:
  stmfd sp!, {r4-r12, lr}	// Stack r4-r12, lr

  add lr, r0, r1			// Store r0 + r1 in lr, this will point to the end of the buffer

  movs r1, #00				// Set r1-r12 to 0
  movs r2, #00
  movs r3, #00
  movs r4, #00
  movs r5, #00
  movs r6, #00
  movs r7, #00
  movs r8, #00
  movs r9, #00
  movs r10, #00
  movs r11, #00
  movs r12, #00

loop_zero:
  stmia	r0!, { r1 - r12 }	// Zero 12 words, increment r0
  cmp r0, lr				// Is r0 >= lr?
  blt loop_zero				// If not, repeat

  ldmfd sp!, {r4-r12, pc}	// Unstack r4-r12, unstack lr value directly to pc, return



