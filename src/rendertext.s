/*
 * rendertext.s
 * Output a text string to scanline buffer
 * Created on: 04/10/2018
 */

.syntax unified

#define FNTD_BTSPRCHAR			0
#define FNTD_BTSPRCHAR_W		5
#define FNTD_BYTSPRCHR			5
#define FNTD_BYTSPRCHR_W		3
#define FNTD_SCALE				8
#define FNTD_SCALE_W			2
#define FNTD_BTSPRLIN			10
#define FNTD_BTSPRLIN_W			5
#define FNTD_CURLIN				15
#define FNTD_CURLIN_W			5
#define FNTD_COLOUR				24
#define FNTD_COLOUR_W			8

#define LB_RECT_LEFT			2
#define LB_RECT_TOP				4
#define LB_RECT_WIDTH			6
#define LB_COLOUR				11
#define LB_FONT					16
#define LB_SCALE				20
#define LB_TEXT					24

#define FONT_BTSPRCHAR			0
#define FONT_BTSPRLIN			1
#define FONT_BYTSPRCHR			2
#define FONT_DATA				4

.section .text
.global rendertext
.type  rendertext, %function								// r0: pointer to LABEL structure
															// renderBuf and currentRenderScanLine are externals

pl		.req r0												// ppm (pointer to label struct)
pdst	.req r1												// pdst (pointer to dest)
psrc 	.req r2												// psrc (pointer to source bitmap)
fntdt	.req r3												// font details (width in bits [0-4], bytes per char line [5-6], scale [7-8], height in bits [9-13], current line [14-18], colour [24-31])
src		.req r4												// src (current font bitmap byte)

width   .req r10
srcbit	.req r12											// counters
dstbyte .req r7
sclcnt	.req r8

psz		.req r9												// pointer to chars to render

mask	.req r5												// mask for blitting
dest	.req r11											// bits to blit to pdst
tmp		.req r6												// scratch reg
tmp2	.req lr												// scratch reg

rendertext:
  stmfd sp!, {r4-r12, lr}									// Stack r4-r12, lr

initialize:
  ldr pdst, =renderBuf										// pdst = &renderBuf
  ldr pdst, [pdst]											// pdst = renderBuf
  ldrh tmp, [pl, #LB_RECT_LEFT]
  add pdst, tmp												// pdst = renderBuf + pdst->hdr.rect.left

  ldr tmp2, =currentRenderScanLine
  ldrh tmp, [tmp2]											// tmp = *currentRenderScanLine
  ldrh tmp2, [pl, #LB_RECT_TOP]								// tmp2 = pl->hdr.rect.top
  sub tmp, tmp2												// tmp -= tmp2
  ldrb tmp2, [pl, #LB_SCALE]								// tmp2 = pl->scale
  udiv tmp, tmp, tmp2										// tmp /= tmp2
  bfi fntdt, tmp, #FNTD_CURLIN, #FNTD_CURLIN_W				// line, after scaling

  ldr psrc, [pl, #LB_FONT]									// psrc = pdst->font
  ldr psz, [pl, #LB_TEXT]									// psz = pdst->text

  mov tmp2, tmp
  ldrb tmp, [psrc, #FONT_BTSPRLIN]
  bfi fntdt, tmp, #FNTD_BTSPRLIN, #FNTD_BTSPRLIN_W

  //cmp tmp2, tmp												// if(line >= btsPerLine) goto return
  //bge return

  ldrb tmp, [psrc, #FONT_BTSPRCHAR]
  bfi fntdt, tmp, #FNTD_BTSPRCHAR, #FNTD_BTSPRCHAR_W
  ldrb tmp, [psrc, #FONT_BYTSPRCHR]
  bfi fntdt, tmp, #FNTD_BYTSPRCHR, #FNTD_BYTSPRCHR_W
  ldrb tmp, [pl, #LB_COLOUR]
  bfi fntdt, tmp, #FNTD_COLOUR, #FNTD_COLOUR_W
  ldrb tmp, [pl, #LB_SCALE]
  bfi fntdt, tmp, #FNTD_SCALE, #FNTD_SCALE_W

  ldrh width, [pl, #LB_RECT_WIDTH]
  ubfx sclcnt, fntdt, #FNTD_SCALE, #FNTD_SCALE_W			// sclcnt = pl->scale
  mov srcbit, #0											// todo: offset if we are right-aligned and clipping
  ubfx dstbyte, pdst, #0, #2								// dstbyte = 2 lsb of pdst
  and pdst, #0xfffffffc										// pdst now word-aligned

  ldr psrc, [psrc, #FONT_DATA]								// psrc = psrc->data

  // Todo: offset psz, srcbit etc if we are right-aligned and clipping

  // Get first char
  ldrb tmp, [psz], #1										// tmp = *psz++
  teq tmp, #0
  beq return												// if(!tmp) goto return

  sub tmp, #32												// tmp -= 32, subtract 32 as our font doesn't include ctrl chars
  ubfx tmp2, fntdt, #FNTD_BTSPRLIN, #FNTD_BTSPRLIN_W		// tmp2 = pl->font->charheight
  mul tmp, tmp2												// tmp *= tmp2
  ubfx tmp2, fntdt, #FNTD_CURLIN, #FNTD_CURLIN_W			// tmp2 = current line
  add tmp, tmp2												// tmp += tmp2
  ldrb src, [psrc, tmp]										// src = *(psrc + tmp) !!! This might be unaligned

loop:
  // Get pixel value
  mov tmp, #0x80
  ror tmp, tmp, srcbit										// tmp = 0x80 >> srcbit
  and tmp, src												// tmp &= src
  teq tmp, #0x00
  ite eq													// tmp = (tmp == 0) ? 0 : 0xff
  moveq tmp, #0x00
  movne tmp, #0xff

  mov tmp2, #4
  sub tmp2, dstbyte
  lsl tmp2, 3												// tmp2 = (4 - dstbyte) * 8

  lsl tmp, tmp2												// tmp <<= tmp2
  orr mask, tmp												// mask |= tmp

  teq tmp, #0x00											// if (tst == 9)
  ittt ne													// {
  ubfxne tmp, fntdt, #FNTD_COLOUR, #FNTD_COLOUR_W			//    tmp = colour
  lslne tmp, tmp2											//	  tmp <<= tmp2
  orrne dest, tmp											//	  dest |= tmp }

  add dstbyte, #1
  cmp dstbyte, #4
  bne done_blit

blit:
  mov dstbyte, #0											// dstbyte = 0
  cbz mask, done_blit										// if(mask == 0) goto done_blit
  cmp mask, #0xffffffff										// if(mask != 0xffffffff) goto read_then_modify_blit
  bne read_then_modify_blit

direct_bit:
  str dest, [pdst], #4										// *pdst = dest; pdst += 4;
  b done_blit

read_then_modify_blit:
  ldr tmp, [pdst], #0										// tmp = *pdst
  mov tmp2, #0xffffffff
  bic tmp2, mask											// tmp2 &= ~mask
  and tmp, tmp2												// tmp &= tmp2
  orr tmp, dest												// tmp |= dest
  str tmp, [pdst], #4										// *pdst = tmp; pdst += 4;

done_blit:
  subs sclcnt, #1											// if(--sclcnt != 0) goto continue_looop
  bne continue_loop

advance_next_bit:
  add srcbit, #1											//srcbit++
  ubfx sclcnt, fntdt, #FNTD_SCALE, #FNTD_SCALE_W			// sclcnt = pl->scale
  ubfx tmp, fntdt, #FNTD_BTSPRCHAR, #FNTD_BTSPRCHAR_W		// tmp = width (todo: will need to increase this because this gives max 15)
  cmp srcbit, tmp
  bne continue_loop											// if(srcbit != width) goto continue_looop

get_next_char:
  mov srcbit, #0
  ldrb tmp, [psz], #1										// tmp = *psz++
  cbz tmp, return											// if(!tmp) goto return

  sub tmp, #32												// tmp -= 32
  ubfx tmp2, fntdt, #FNTD_BTSPRLIN, #FNTD_BTSPRLIN_W		// tmp2 = pl->font->charheight
  mul tmp, tmp2												// tmp *= tmp2
  ubfx tmp2, fntdt, #FNTD_CURLIN, #FNTD_CURLIN_W			// tmp2 = current line
  add tmp, tmp2												// tmp += tmp2
  ldrb src, [psrc, tmp]										// src = *(psrc + tmp) !!! this might be unaligned

continue_loop:
  subs width, #1											// if(--width > 0) goto loop else return
  bne loop

return:
  ldmfd sp!, {r4-r12, pc}									// Unstack r4-r12, unstack lr value directly to pc, return




