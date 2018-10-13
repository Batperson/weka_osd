/*
 * zerobuf.s
 * Routine for quickly zeroing a buffer, which must be word-aligned and a multiple of 128 bytes in size.
 * Created on: 04/10/2018
 */

.syntax unified

.section .text
.global zerobuf
.type  zerobuf, %function	// r0: buffer start (word aligned), r1: buffer length (multiple of 128)

zerobuf:
  vpush { d8 - d15 }		// Stack the floating point registers which need to be preserved.

  add r1, r0, r1			// Store r0 + r1 in r1, this will point to the end of the buffer

  movs r2, #00				// Load r2 with 0
  vmov s0, r2				// Load floating point registers with r2
  vmov s1, s0
  vmov s2, s0
  vmov s3, s0
  vmov s4, s0
  vmov s5, s0
  vmov s6, s0
  vmov s7, s0
  vmov s8, s0
  vmov s9, s0
  vmov s10, s0
  vmov s11, s0
  vmov s12, s0
  vmov s13, s0
  vmov s14, s0
  vmov s15, s0
  vmov s16, s0
  vmov s17, s0
  vmov s18, s0
  vmov s19, s0
  vmov s20, s0
  vmov s21, s0
  vmov s22, s0
  vmov s23, s0
  vmov s24, s0
  vmov s25, s0
  vmov s26, s0
  vmov s27, s0
  vmov s28, s0
  vmov s29, s0
  vmov s30, s0
  vmov s31, s0

loop_zero:
  vstmia r0!, { d0 - d15 }	// Write 128 bytes, increment r0
  cmp r0, r1				// Is r0 >= r1?
  blt loop_zero				// If not, repeat

  vpop { d8 - d15 }			// Unstack the floating point registers saved earlier
  bx lr						// return
