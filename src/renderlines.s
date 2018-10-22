/*
 * rendertext.s
 * Output an array of lines to scanline buffer
 * Created on: 20/10/2018
 */

.syntax unified

#define SIZEOF_LINE					8

#define LN_P1_X						0
#define LN_P1_Y						2
#define LN_P2_X						4
#define LN_P2_Y						6

#define SP_FLAGS
#define SP_RECT_LEFT				2
#define SP_RECT_TOP					4
#define SP_RECT_WIDTH				6
#define SP_COLOUR					11



.section .text
.global renderlines
.type  renderlines, %function								// r0: pointer to array of LINE structures
															// r1: count of array

ps		.req r0												// pointer to sprite
pl		.req r1												// pointer to array of lines
cnt		.req r2												// count of lines

pdst	.req r4
tmp		.req r5												// scratch reg
tmp2	.req r6												// scratch reg
line	.req r7
x		.req r8
y		.req r9
dx		.req r10
dy		.req r11

renderlines:
  stmfd sp!, {r4-r12, lr}									// Stack r4-r12, lr

initialize:
  ldr pdst, =renderBuf
  ldrh tmp, [ps, #SP_RECT_LEFT]
  add pdst, tmp												// pdst = renderBuf + pdst->hdr.rect.left

  ldr tmp2, =currentRenderScanLine
  ldrh tmp, [tmp2]											// tmp = *currentRenderScanLine
  ldrh tmp2, [ps, #SP_RECT_TOP]								// tmp2 = ps->rect.top
  sub tmp, tmp2												// tmp -= tmp2
  mov line, tmp												// line = tmp

loop:
  ldrh tmp, [pl, #LN_P1_X]
  ldrh tmp2, [pl, #LN_P2_X]
  teq tmp, tmp2
  bne render_nonvertical_line								// if(pl->p1.x != pl->p2.x) goto render_nonvertical_line

render_vertical_line:
  ldrh tmp2, [ps, #SP_RECT_LEFT]
  cmp tmp, tmp2
  blt continue_loop											// if(pl->p1.x < ps->rect.left) goto continue_loop

  ldrh x, [ps, #SP_RECT_WIDTH]
  add tmp2, x												// tmp2 += ps->rect.width)
  cmp tmp, tmp2
  bge continue_loop											// if(pl->p1.x >= ps->rect.width) goto continue_loop

render_nonvertical_line:

continue_loop:
  add pl, #SIZEOF_LINE
  subs cnt, #1												// if(--cnt > 0) goto loop
  bne loop

return:
  ldmfd sp!, {r4-r12, pc}									// Unstack r4-r12, unstack lr value directly to pc, return




