/*
 * rasterize.c
 *
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "graphics.h"

PIXEL  lineBuf0[1024] IN_CCM ALIGNED(1024);
PIXEL  lineBuf1[1024] IN_CCM ALIGNED(1024);
PPIXEL renderBuf IN_CCM;

void rasterizeNextScanLine()
{
	// For now, just generate a test pattern.

	PIXEL colourBars[] = { RED, GREEN, BLUE, TRANSPARENT, WHITE, RGBA(0,0,0,1), RGB(1,1,1), RGB(2,2,2), RGB(3,3,3), RGBA(0,0,0,1) };
	PPIXEL pos = renderBuf + 100;

	for(u32 i=0; i<sizeof(colourBars); i++)
	{
		for(u32 j=0; j<40; j++)
		{
			*pos++ = colourBars[i];
		}
	}
}
