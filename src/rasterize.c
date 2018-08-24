/*
 * rasterize.c
 *
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "graphics.h"
#include "sprite.h"

PIXEL  lineBuf0[1024] IN_CCM ALIGNED(1024);
PIXEL  lineBuf1[1024] IN_CCM ALIGNED(1024);
volatile PPIXEL renderBuf IN_CCM;

SPRITE testpattern;

PSPRITE sprites[10];

void rasterizeNextScanLine()
{
	// For now, just generate a test pattern.
	PIXEL colourBars[] = { RED, GREEN, BLUE, WHITE };//, RGBA(0,0,0,1), TRANSPARENT, RGB(1,1,1) /*, RGB(2,2,2), RGB(3,3,3), RGBA(0,0,0,1)*/ };
	PPIXEL pos = renderBuf;

	for(u32 i=0; i<sizeof(colourBars); i++)
	{
		for(u32 j=0; j<20; j++)
		{
			*pos++ = colourBars[i];
		}
	}

	/*
	u32 width = 80;// 768;

	for(u32 i=0; i<width; i++)
		*pos++ = WHITE;
	*/
}
