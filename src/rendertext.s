/*
 * rendertext.s
 * Output a text string to scanline buffer
 * Created on: 04/10/2018
 */

.syntax unified

.section .text
.global rendertext
.type  rendertext, %function	// r0: pointer to render parameters (colour, clip rect, flags)
								// r1: pointer to font
								// r2: pointer to chars to render, length-prefixed
								// r3:
								// renderBuf and currentRenderScanLine are externals

ppm		.req r0					// ppm (pointer to render params) is r0
pfnt	.req r1					// pfnt (pointer to font) is r1
pstr 	.req r2					// pstr (pointer to string, length-prefixed) is r2
strlen	.req r4					// strlen is r4
scale	.req r5					// scale is r5
colour	.req r6
soffsx	.req r7					// source offset x
soffsy	.req r8					// source offset y
chrh	.req r9					// char height

pbuf	.req r10				// pbuf (pointer to current position in output buffer) is r10
pclprt	.req lr					// pclprt (pointer to buffer position of right of clip window) is lr

  // Test code & initialization
  mov scale, #2					// test code, set scale = 2.
  mov colour, #111				// test code
  mov soffsx, #0
  mov soffsy, #0
  mov pbuf, #100
  mov pclprt, #200
  mov chrh, #9

rendertext:
  stmfd sp!, {r4-r12, lr}		// Stack r4-r12, lr
  ldr strlen, [pstr], #1		// Load strlen, increment pstr

loop_chars:
  // Get font data position
  ldr r12, [pstr], #1			// Get value of current char, increment pstr
  sub r12, #0x20				// Subtract 32
  mul r12, r12, chrh			// Multiply by char height to get the index into font data
  add r12, pfnt, r12			// Add this index to pfnt
  add r12, soffsy				// Add the current Y offset, r12 now points to the source byte we want in the font bitmap
  ldrb r11, [r12]				// Read that byte into r11

  // Read the bit at the current font position
  mov r12, r11
  lsr r12, soffsx
  tst r12, #0x01

  // If bit not set, skip forward [scale] times
  it ne
  addne pbuf, scale				// Skip pbuf forward [scale] bytes if the bit is not set

  // If bit is set, loop [scale] times writing [colour] to [renderBuf] at the appropriate offset.

  // If we advance past the width of the clip rectangle, return immediately
  cmp pbuf, pclprt
  bgt return

  // If we advance past the width of the current char, set soffsx to 0 and continue looping

  subs strlen, #1				// Decrement strlen
  bne loop_chars				// If not zero, goto loop_chars

return:
  ldmfd sp!, {r4-r12, pc}		// Unstack r4-r12, unstack lr value directly to pc, return




