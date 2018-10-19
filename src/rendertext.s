/*
 * rendertext.s
 * Output a text string to scanline buffer
 * Created on: 04/10/2018
 */

.syntax unified

.section .text
.global rendertext
.type  rendertext, %function	// r0: pointer to LABEL structure
								// renderBuf and currentRenderScanLine are externals

pl		.req r0					// ppm (pointer to label)
pdst	.req r1					// pdst (pointer to dest)
psrc 	.req r2					// psrc (pointer to source bitmap)
fntdt	.req r3					// font details (width in bits [0-4], bytes per char line [5-6], scale [7-8], height in bits [9-13], current line [14-18], colour [24-31])
src		.req r4					// src (current font bitmap byte)

width   .req r5
srcbit	.req r6					// counters
dstbyte .req r7
sclcnt	.req r8

psz		.req r9					// pointer to chars to render

mask	.req r10				// mask for blitting
dest	.req r11				// bits to blit to pdst
tmp		.req r12				// scratch reg
tmp2	.req lr					// scratch reg

rendertext:
  stmfd sp!, {r4-r12, lr}		// Stack r4-r12, lr

initialize:
  ldr pdst, =renderBuf
  ldrh tmp, [pl, #2]
  add pdst, tmp					// pdst = renderBuf + pdst->hdr.rect.left

  ldr tmp2, =currentRenderScanLine
  ldrh tmp, [tmp2]				// tmp = *currentRenderScanLine
  ldrh tmp2, [pl, #4]			// tmp2 = pl->hdr.rect.top
  sub tmp, tmp2					// tmp -= tmp2
  ldrb tmp2, [pl, #20]			// tmp2 = pl->scale
  udiv tmp, tmp, tmp2			// tmp /= tmp2
  bfi fntdt, tmp, #14, #5		// line, after scaling

  ldr psrc, [pl, #16]			// psrc = pdst->font
  ldr psz, [pl, #24]			// psz = pdst->text

  ldrb tmp, [psrc, #0]
  bfi fntdt, tmp, #0, #4		// width (bits per line)
  ldrb tmp, [psrc, #1]
  bfi fntdt, tmp, #5, #2		// bytes per line
  ldrb tmp, [psrc, #2]
  bfi fntdt, tmp, #9, #5		// height
  ldrb tmp, [pl, #11]
  bfi fntdt, tmp, #24, #8		// colour
  ldrb tmp, [pl, #20]
  bfi fntdt, tmp, #7, #2		// scale

  ldrh width, [pl, #6]
  ubfx sclcnt, fntdt, #7, #2	// sclcnt = pl->scale
  mov srcbit, #0				// todo: offset if we are right-aligned and clipping
  ubfx dstbyte, pdst, #0, #2	// dstbyte = 2 lsb of pdst
  and pdst, #0xfffffffc			// pdst now word-aligned

  ldr psrc, [psrc, #4]			// psrc = psrc->data

  // Todo: offset psz, srcbit etc if we are right-aligned and clipping

  // Get first char
  ldrb tmp, [psz], #1			// tmp = *psz++
  teq tmp, #0
  beq return					// if(!tmp) goto return

  sub tmp, #32					// tmp -= 32
  ubfx tmp2, fntdt, #9, #4		// tmp2 = pl->font->charheight
  mul tmp, tmp2					// tmp *= tmp2
  ubfx tmp2, fntdt, #14, #5		// tmp2 = current line
  add tmp, tmp2					// tmp += tmp2
  ldrb src, [psrc, tmp]			// src = *(psrc + tmp) !!! This might be unaligned

loop:
  // Get pixel value
  mov tmp, #0x80
  ror tmp, tmp, srcbit			// tmp = 0x80 >> srcbit
  and tmp, src					// tmp &= src
  teq tmp, #0x00
  ite eq						// tmp = (tmp == 0) ? 0 : 0xff
  moveq tmp, #0x00
  movne tmp, #0xff

  mov tmp2, #4
  sub tmp2, dstbyte
  lsl tmp2, 3					// tmp2 = (4 - dstbyte) * 8

  lsl tmp, tmp2					// tmp <<= tmp2
  orr mask, tmp					// mask |= tmp

  teq tmp, #0x00				// if (tst == 9)
  ittt ne						// {
  ubfxne tmp, fntdt, #24, #8	//    tmp = colour
  lslne tmp, tmp2				//	  tmp <<= tmp2
  orrne dest, tmp				//	  dest |= tmp }

  add dstbyte, #1
  cmp dstbyte, #4
  bne skip_over_blit

blit:
  mov dstbyte, #0				// dstbyte = 0
  cmp mask, #0x00				// if(mask == 0) goto done_blit
  beq done_blit
  cmp mask, #0xffffffff			// if(mask != 0xffffffff) goto read_then_modify_blit
  bne read_then_modify_blit

direct_bit:
  str dest, [pdst], #4			// *pdst = dest; pdst += 4;
  b done_blit

read_then_modify_blit:
  ldr tmp, [pdst], #0			// tmp = *pdst
  mov tmp2, #0xffffffff
  bic tmp2, mask				// tmp2 &= ~mask
  and tmp, tmp2					// tmp &= tmp2
  orr tmp, dest					// tmp |= dest
  str tmp, [pdst], #4			// *pdst = tmp; pdst += 4;

done_blit:
  subs sclcnt, #1				// if(--sclcnt != 0) goto continue_looop
  bne continue_loop

advance_next_bit:
  add srcbit, #1				//srcbit++
  ubfx tmp, fntdt, #0, #4		// tmp = width (todo: will need to increase this because this gives max 15)
  cmp srcbit, tmp
  bne continue_looop		// if(srcbit != width) goto continue_looop

get_next_char:
  mov srcbit, #0
  ldrb tmp, [psz], #1			// tmp = *psz++
  teq tmp, #0
  beq return					// if(!tmp) goto return

  sub tmp, #32					// tmp -= 32
  ubfx tmp2, fntdt, #9, #4		// tmp2 = pl->font->charheight
  mul tmp, tmp2					// tmp *= tmp2
  ubfx tmp2, fntdt, #14, #5		// tmp2 = current line
  add tmp, tmp2					// tmp += tmp2
  ldrb src, [psrc, tmp]			// src = *(psrc + tmp) !!! this might be unaligned

continue_loop:
  subs width, #1				// if(--width > 0) goto loop else return
  bne loop

return:
  ldmfd sp!, {r4-r12, pc}		// Unstack r4-r12, unstack lr value directly to pc, return




