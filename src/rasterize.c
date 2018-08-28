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

extern volatile PPIXEL renderBuf;

SPRITE testpattern;

PSPRITE sprites[10];

void rasterizeNextScanLine()
{
	//memset(renderBuf, 0, LINE_BUFFER_SIZE);

	// For now, just generate a test pattern.
	PIXEL colourBars[] = { SEMITRANSPARENT, RED, ORANGE, YELLOW, GREEN, PURPLE, BLUE, WHITE, SEMITRANSPARENT  };
	PPIXEL pos = renderBuf + 40;

	for(u32 i=0; i<sizeof(colourBars); i++)
	{
		for(u32 j=0; j<30; j++)
		{
			*pos++ = colourBars[i];
		}
	}
}
