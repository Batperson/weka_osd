/*
 * graphics.c
 *
 */

#include "stm32f4xx.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "graphics.h"
#include "memory.h"
#include "video.h"

extern volatile PPIXEL currentRenderBuf;

ALWAYS_INLINE PPIXEL ptToOffset(DU x, DU y) { return currentRenderBuf + (y * FRAME_BUF_WIDTH) + x; }
ALWAYS_INLINE void setPixel(DU x, DU y, COLOUR colour) { *ptToOffset(x,y) = colour; }

static COLOUR testPattern[]= { RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, VIOLET, WHITE, GRAY, DKGRAY };
static RECT rcBuf = { 0, 0, FRAME_BUF_WIDTH, FRAME_BUF_HEIGHT };

void clearRenderBuf()
{
	//zerobuf(currentRenderBuf, FRAME_BUF_SIZE);
	memset(currentRenderBuf, 0, FRAME_BUF_SIZE);
}

void offsetPts(PPOINT ppt, u16 cnt, DU dx, DU dy)
{
	while(cnt--)
	{
		PPOINT pt = ppt+cnt;

		pt->x += dx;
		pt->y += dy;
	}
}

void rotatePts(PPOINT ppt, u16 cnt, PPOINT ctr, float angle)
{
	angle *= (M_PI / 180);

	while(cnt--)
	{
		PPOINT pt = ppt+cnt;
		float x = pt->x - ctr->x;
		float y = pt->y - ctr->y;

		float sin = sinf(angle);
		float cos = cosf(angle);

		pt->x		= truncf((x * cos) - (y * sin)) + ctr->x;
		pt->y		= truncf((y * cos) + (x * sin)) + ctr->y;
	}
}

void drawTestPattern(PRECT rect)
{
	int bands 		= sizeof(testPattern) / sizeof(COLOUR);
	int bandWidth 	= rect->width / bands;
	if(bandWidth == 0)
		bandWidth = 1;

	for(int i=rect->top; i<rect->top + rect->height; i++)
		for(int j=0; j<bands;j++)
			mset(ptToOffset(rect->left + (j * bandWidth), i), testPattern[j], bandWidth);
}

void drawRect(PRECT rect, COLOUR foreground, COLOUR background)
{
	DU bottom = rect->top + rect->height - 1;
	mset(ptToOffset(rect->left, rect->top), foreground, rect->width);

	for(DU i=rect->top + 1; i<bottom; i++)
	{
		setPixel(rect->left, i, foreground);
		mset(ptToOffset(rect->left + 1, i), background, rect->width - 1);
		setPixel(rect->left + rect->width-1, i, foreground);
	}

	mset(ptToOffset(rect->left, bottom), foreground, rect->width);
}

void drawArrow(PRECT rect, COLOUR foreground, COLOUR background, AlignmentType alignment)
{
	DU bt	= rect->top + rect->height - 1;
	DU ao	= rect->height >> 1;
	DU l 	= (alignment == AlignRight) ? rect->left + ao : rect->left;
	DU w 	= rect->width - ao;
	DU mp	= rect->top + ao;

	mset(ptToOffset(l, rect->top), foreground, w);
	mset(ptToOffset(l, bt), foreground, w);

	for(DU i=rect->top + 1; i<bt; i++)
	{
		DU z 	= abs(mp - i) + 1;
		l 		= (alignment == AlignRight) ? rect->left + z : rect->left;
		w		= rect->width - z;

		setPixel(l, i, foreground);
		mset(ptToOffset(l+1, i), background, w - 1);
		setPixel(l+w, i, foreground);
	}
}

void drawLine(PLINE line, COLOUR foreground, PRECT clip)
{
	if(clip == NULL) clip = &rcBuf;

	DU dx, dy, e;
	DU incx, incy, inc1, inc2;
	DU x, y;

	dx = line->p2.x - line->p1.x;
	dy = line->p2.y - line->p1.y;

	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;
	incx = 1;

	if(line->p2.x < line->p1.x) incx = -1;
	incy = 1;

	if(line->p2.y < line->p1.y) incy = -1;

	x=line->p1.x;
	y=line->p1.y;

	if(dy == 0)
	{
		if(line->p1.y >= clip->top && line->p1.y < clip->top + clip->height)
		{
			if(x > line->p2.x) x = line->p2.x;
			if(x < clip->left) x = clip->left;
			if(x + dx > clip->left + clip->width) dx -= ((x + dx) - (clip->left + clip->width));

			mset(ptToOffset(x, line->p1.y), foreground, dx);
		}
	}
	else if(dx == 0)
	{
		if(line->p1.x >= clip->left && line->p1.x < clip->left + clip->width)
		{
			if(y > line->p2.y) y = line->p2.y;
			if(y < clip->top) y = clip->top;
			if(y + dy > clip->top + clip->height) dx -= ((x + dx) - (clip->top + clip->height));

			dy += y;

			for (DU i = y; i < dy; i++)
				setPixel(x, i, foreground);
		}
	}
	else if (dx > dy)
	{
		if(ptInRect(clip, x, y))
			setPixel(x, y, foreground);

		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;

		for (DU i = 0; i < dx; i++)
		{
			if (e >= 0)
			{
				y += incy;
				e += inc1;
			}
			else
			{
				e += inc2;
			}

			x += incx;

			if(ptInRect(clip, x, y))
				setPixel(x, y, foreground);
		}
	}
	else
	{
		if(ptInRect(clip, x, y))
			setPixel(x, y, foreground);

		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;

		for(DU i = 0; i < dy; i++)
		{
			if (e >= 0)
			{
				x += incx;
				e += inc1;
			}
			else
			{
				e += inc2;
			}

			y += incy;

			if(ptInRect(clip, x, y))
				setPixel(x, y, foreground);
		}
	}
}

void drawLines(PLINE line, u16 cnt, COLOUR foreground, PRECT clip)
{
	while(cnt--)
		drawLine(line+cnt, foreground, clip);
}
