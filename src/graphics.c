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

static COLOUR foreground = RGB(3,3,3);
static COLOUR background = RGB(0,0,0);

COLOUR selectForeColour(COLOUR fg)
{
	COLOUR retval = foreground;
	foreground = fg;

	return retval;
}

COLOUR selectBackColour(COLOUR bg)
{
	COLOUR retval = background;
	background = bg;

	return retval;
}

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

void drawRect(PRECT rect, DrawFlags flags)
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

void drawArrow(PRECT rect, DrawFlags alignment)
{
	DU bt	= rect->top + rect->height - 1;
	DU ao	= rect->height >> 1;
	DU l 	= (alignment & AlignRight) ? rect->left + ao : rect->left;
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

void drawLine(PLINE line, DrawFlags flags, PRECT clip)
{
	drawLines(line, 1, foreground, clip);
}

void drawLines(PLINE line, u16 cnt, DrawFlags flags, PRECT clip)
{
	while(cnt--)
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

		line++;
	}
}

void drawText(PRECT rect, PFONT font, DrawFlags flags, char* text)
{
	DU x, y;
	char* sz;
	int sx, sy;

	if(flags & AlignRight)
	{
		int l = strlen(text);
		DU w  = l * font->charwidth;

		x = (rect->left + rect->width) - w;
		y = rect->top;

		if(x < rect->left)
		{
			sz = text + (w / font->charwidth);
			sx = (w % font->charwidth);
		}
		else
		{
			sz = text;
			sx = 0;
		}
	}
	else
	{
		sx = 0;
		sz = text;
		x = rect->left;
		y = rect->top;
	}

	if(flags & AlignBottom)
	{
		DU d;
		y = (rect->top + rect->height) - font->charheight;
		if((d = y - rect->top) < 0)
			sy = -d;
	}
	else
	{
		sy = 0;
	}

	COLOUR fg 	= (flags & Inverse) ? background : foreground;
	COLOUR bg 	= (flags & Inverse) ? foreground : background;
	u32 bbrsh 	= bg | bg << 8 | bg << 16 | bg << 24;
	u32 defmask	= (flags & Inverse) ? 0xffffffff : 0x00;

	while(*sz)
	{
		for(int i=sy; i<font->charheight; i++)
		{
			PPIXEL dest = ptToOffset(x, y + i);

			// Note will have issues if we have a font with chars more than 8 wide and we start at sx >= 8

			register u8* src	= font->data + ((*sz-font->asciiOffset) * font->charheight) + (i * font->bytesPerLine);
			register u32* dw	= (u32*)((u32)dest & ~0x3);	// Destination word, the frame buffer word we are blitting to
			register u8 dwp		= ((u32)dest & 0x3);		// Destination work position, the byte index within dw for the current byte we are setting
			register u32 v		= 0x80 >> sx;				// The bit in src that we expect to see set if there is an active pixel
			register u32 data	= bbrsh;					// The buffer that will be combined with [dest] using [mask]
			register u32 mask	= defmask << (dwp * 8);

			for(int j=sx; j<font->charwidth; j++)
			{
				if((x + j) >= (rect->left + rect->width))
					break;

				register u8 masked 		= 0;			// if 1, then we will plot a pixel
				register u32 shift 		= (dwp * 8);	// How much to left-shift to reach the current pixel position in the current word

				// Note: [shift] must take account of little-endian word arrangement, i.e. left-most pixel goes at right-most byte, so it is a left shift not a right.
				if(flags & Outline)
				{
					if(j > 0 && (*src & v << 1))
						masked = 1;
					else if(j < font->charwidth-1 && (*src & v >> 1))
						masked = 1;
					else if(i > 0 && (*(src - font->bytesPerLine) & v))
						masked = 1;
					else if(i < font->charheight-1 && (*(src + font->bytesPerLine) & v))
						masked = 1;
				}

				if(*src & v)
				{
					masked = 1;
					data &= ~(0xff << shift);
					data |= (fg << shift);
				}

				if(masked == 1)
					mask |= (0xff << shift);

				if(++dwp >= 4)
				{
					if(mask == 0xffffffff)
						*dw = data;
					else if(mask > 0)
						*dw = ((*dw & ~mask) | (data & mask));

					dwp 	= 0;
					mask 	= defmask;
					data	= bbrsh;
					dw++;
				}

				if(!(v >>= 1))
				{
					v = 0x80;
					src++;
				}
			}

			if(dwp > 0)
			{
				if(mask == 0xffffffff)
					*dw = data;
				else if(mask > 0)
					*dw = ((*dw & ~mask) | (data & mask));
			}

			if((y + i) >= (rect->top + rect->height))
				break;
		}

		sx = 0;
		sz++;

		if((x += font->charwidth) >= rect->left + rect->width)
			break;
	}
}

