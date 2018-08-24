/*
 * rasterize.c
 *
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "graphics.h"
#include "video.h"
#include "rasterize.h"
#include "sprite.h"

extern volatile PPIXEL renderBuf IN_CCM;

SPRITE testpattern;

PSPRITE sprites[10];

void rasterizeNextScanLine()
{
	//memset(renderBuf, 0, LINE_BUFFER_SIZE);

	// For now, just generate a test pattern.
	PIXEL colourBars[] = { RED, ORANGE, YELLOW, GREEN, PURPLE, BLUE, WHITE, RGBA(0,0,0,1) };
	PPIXEL pos = renderBuf + 4;

	for(u32 i=0; i<sizeof(colourBars); i++)
	{
		for(u32 j=0; j<30; j++)
		{
			*pos++ = colourBars[i];
		}
	}
}
