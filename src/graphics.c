/*
 * graphics.c
 *
 */

#include "stm32f4xx.h"
#include "string.h"
#include "stdio.h"
#include "graphics.h"
#include "memory.h"
#include "video.h"

extern volatile PPIXEL currentRenderBuf;

COLOUR testPattern[]= { RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, VIOLET, WHITE, GRAY, DKGRAY };

void drawTestPattern(PRECT rect)
{
	int bands 		= sizeof(testPattern) / sizeof(COLOUR);
	int bandWidth 	= rect->width / bands;
	if(bandWidth == 0)
		bandWidth = 1;

	for(int i=rect->top; i<rect->top + rect->height; i++)
		for(int j=0; j<bands;j++)
			mset(ptToOffset(currentRenderBuf, rect->left + (j * bandWidth), i), testPattern[j], bandWidth);
}

void drawRect(PRECT rect, COLOUR foreground, COLOUR background)
{
	u16 bottom = rect->top + rect->height - 1;
	mset(ptToOffset(currentRenderBuf, rect->left, rect->top), foreground, rect->width);

	for(int i=rect->top + 1; i<bottom; i++)
	{
		mset(ptToOffset(currentRenderBuf, rect->left + 1, i), background, rect->width - 1);
		*ptToOffset(currentRenderBuf, rect->left, i) = foreground;
		*ptToOffset(currentRenderBuf, rect->left + rect->width-1, i) = foreground;
	}

	mset(ptToOffset(currentRenderBuf, rect->left, bottom), foreground, rect->width);
}
