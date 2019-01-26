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
#include "misc.h"

extern volatile PPIXEL currentRenderBuf;

ALWAYS_INLINE PPIXEL ptToOffset(DU x, DU y) { return currentRenderBuf + (y * FRAME_BUF_WIDTH) + x; }
ALWAYS_INLINE void setPixel(DU x, DU y, COLOUR colour) { *ptToOffset(x,y) = colour; }

static COLOUR testPattern[]= { RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, VIOLET, WHITE, GRAY, DKGRAY };
static RECT rcBuf = { 0, 0, FRAME_BUF_WIDTH, FRAME_BUF_HEIGHT };

static COLOUR foreground = RGB(3,3,3);
static COLOUR background = RGB(0,0,0);

static PIXEL mask[32] ALIGNED(4);
static PIXEL bmap[32] ALIGNED(4);

ALWAYS_INLINE void expand2bpp8(u32* dst, u32* mask, u8 offset, u8* src, u8 cnt, COLOUR* palette)
{
	register u32 mval		= 0xffffffff;
	register u32 dval		= 0x00;
	register u32 sror		= 6;
	register u32 drol		= offset << 3;
	register u32 pindex;
	register u32 tmp;

	for(; cnt > 0; cnt--)
	{
		// src is shifted right based on the inverse of src offset, since left-most palette index is at left-most position in src
		pindex				= ((*src) >> sror) & 0x03;
		if(sror == 0)
		{
			sror = 6;
			src++;
		}
		else
		{
			sror -= 2;
		}

		// dst and mask shifts take account of little-endian word arrangement, i.e. left-most pixel goes at right-most byte
		tmp					= *(palette + pindex) << drol;
		dval 				|= tmp;

		// mask bits are cleared if palette is an opaque colour, set if not.
		tmp					= tmp ? 0xffffff00 : 0xffffffff;
		mval 				&= ROL(tmp, drol);

		if((drol += 8) > 24)
		{
			drol 	= 0;
			*mask++		= mval;
			*dst++		= dval;

			mval		= 0xffffffff;
			dval		= 0x00;
		}
	}

	if(drol > 0)
	{
		*mask		= mval;
		*dst		= dval;
	}
}



/**
  * @brief  Uses a bitblt-style transfer to copy pixels using a mask
  * @param  src: The source bitmap.
  * @param  dst: The destination to copy pixels to.
  * @param mask: The mask to use.
  * @param cnt:  Number of words (4-pixel blocks) to transfer.
  * @retval None
  */
ALWAYS_INLINE void blit(u32* src, u32* dst, u32* mask, u8 cnt)
{
	register u32 tmp;
	for(; cnt > 0; cnt--)
	{
		tmp 		= *dst;
		tmp			&= *mask++;
		tmp			|= *src++;

		*dst++		= tmp;
	}
}

/**
  * @brief  Draws text using the specified font and flags.
  * @param  rect: The location and clip rectangle for rendering. Position within rect is dependent on flags.
  * @param  font: The font to use.
  * @param flags: Controls the style and position used when rendering.
  * @param text: The text to render.
  * @retval None
  */
void drawText(PRECT rect, PFONT font, DrawFlags flags, char* text)
{
	u8 stride	= (font->stride) ? font->stride : font->charwidth;
	DU right	= rect->left + rect->width;
	DU x, y;

	COLOUR palette[4];
	if(flags & Inverse)
	{
		palette[0]	= (flags & Fill) ? 				foreground : TRANSPARENT;
		palette[1]	= (flags & (Outline | Fill)) ? 	foreground : TRANSPARENT;
		palette[2]	= background;
		palette[3]	= background;
	}
	else
	{
		palette[0]	= (flags & Fill) ? 				background : TRANSPARENT;
		palette[1]	= (flags & (Outline | Fill)) ? 	background : TRANSPARENT;
		palette[2]	= foreground;
		palette[3]	= foreground;
	}

	int lstart 	= 0;
	int lend	= font->charheight;

	// Calculate Y position for rendering, and the line within the font bitmap to render from
	if(flags & AlignBottom)
	{
		if(lend > rect->height)
		{
			lstart = lend - rect->height;
			y		= rect->top;
		}
		else
		{
			y		= (rect->top + rect->height) - lend;
		}
	}
	else
	{
		y		= rect->top;
		if(lend > rect->height)
			lend 	= rect->height;
	}

	// Calculate X position to render first character
	if(flags & AlignRight)
	{
		DU textW	= strlen(text) * font->charwidth;
		x 			= right - textW;
	}
	else
	{
		x 			= rect->left;
	}


	// Loop through each character rendering in the foreground colour
	for(char* c = text; *c; c++)
	{
		if(x + stride < rect->left)
			continue;
		if(x >= right)
			break;

		// Byte offset within the destination word in the frame buffer
		u8 ofst = (u8)(u32)ptToOffset(x, 0) & 0x03;

		// Number of words which need to be blitted.
		u8 bcnt = ofst + font->charwidth;
		u8 wcnt	= (bcnt >> 2) + (bcnt & 0x03 ? 1 : 0);

		// Loop through each char bitmap line
		int yoff = 0;
		for(int i=lstart; i<lend; i++)
		{
			u8* src	= font->data + ((((*c-font->asciiOffset) * font->charheight) + i) * font->bytesPerLine);
			u8* dst = (u8*)((u32)ptToOffset(x, y + yoff++) & ~0x03);

			expand2bpp8((u32*)bmap, (u32*)mask, ofst, src, font->charwidth, palette);
			blit((u32*)bmap, (u32*)dst, (u32*)mask, wcnt);
		}

		x += stride;
	}
}

void measureText(PFONT font, char* text, PPOINT sz)
{
	int len 	= strlen(text);
	u8 stride	= (font->stride) ? font->stride : font->charheight;

	sz->x		= (len > 0) ? font->charwidth + ((len-1) * stride) : 0;
	sz->y		= font->charheight;
}

void initRect(PRECT rc, DU left, DU top, DU width, DU height)
{
	rc->left 	= left;
	rc->top		= top;
	rc->width	= width;
	rc->height	= height;
}

void inflateRect(PRECT rc, DU width, DU height)
{
	rc->left	-= width;
	rc->width	+= (width * 2);
	rc->top		-= height;
	rc->height	+= (height * 2);
}

void offsetRect(PRECT rc, DU left, DU top)
{
	rc->left	+= left;
	rc->top		+= top;
}

void moveRect(PRECT rc, DU left, DU top)
{
	rc->left	= left;
	rc->top		= top;
}

void sizeRect(PRECT rc, DU width, DU height)
{
	rc->width	= width;
	rc->height	= height;
}

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
	DU top 		= rect->top;
	DU bottom	= rect->top + rect->height - 1;
	COLOUR fore	= (flags & Inverse) ? background : foreground;
	COLOUR back	= (flags & Inverse) ? foreground : background;

	if(flags & Outline)
	{
		mset(ptToOffset(rect->left, rect->top), fore, rect->width);
		mset(ptToOffset(rect->left, bottom), fore, rect->width);
		top++;
		bottom--;
	}

	for(DU i = top; i<=bottom; i++)
	{
		if(flags & Fill)
			mset(ptToOffset(rect->left, i), back, rect->width);

		if(flags & Outline)
		{
			setPixel(rect->left, i, fore);
			setPixel(rect->left + rect->width-1, i, fore);
		}
	}
}

void drawLinesImpl(PPOINT pt, u16 cnt, u16 incr, DrawFlags flags, PRECT clip)
{
	COLOUR fore = (flags & Inverse) ? background : foreground;

	while(cnt > 0)
	{
		PLINE line = (PLINE)pt;

		if(clip == NULL) clip = &rcBuf;

		DU dx, dy, e;
		DU incx, incy, inc1, inc2;
		DU x, y;

		dx = line->p2.x - line->p1.x;
		dy = line->p2.y - line->p1.y;

		if(dx < 0) dx = -dx;
		if(dy < 0) dy = -dy;

		incx = (line->p2.x < line->p1.x) ? -1 : 1;
		incy = (line->p2.y < line->p1.y) ? -1 : 1;

		x=line->p1.x;
		y=line->p1.y;

		if(dy == 0)
		{
			if(y >= clip->top && y < clip->top + clip->height)
			{
				if(x > line->p2.x) x = line->p2.x;
				if(x < clip->left) x = clip->left;
				if(x + dx > clip->left + clip->width) dx -= ((x + dx) - (clip->left + clip->width));

				mset(ptToOffset(x, y), fore, dx);
			}
		}
		else if(dx == 0)
		{
			if(x >= clip->left && x < clip->left + clip->width)
			{
				if(y > line->p2.y) y = line->p2.y;
				if(y < clip->top) y = clip->top;
				if(y + dy > clip->top + clip->height) dx -= ((x + dx) - (clip->top + clip->height));

				dy += y;

				for (DU i = y; i < dy; i++)
					setPixel(x, i, fore);
			}
		}
		else if (dx > dy)
		{
			if(ptInRect(clip, x, y))
				setPixel(x, y, fore);

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
					setPixel(x, y, fore);
			}
		}
		else
		{
			if(ptInRect(clip, x, y))
				setPixel(x, y, fore);

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
					setPixel(x, y, fore);
			}
		}

		cnt--;
		pt += incr;
	}
}

void drawLine(PLINE line, DrawFlags flags, PRECT clip)
{
	drawLinesImpl((PPOINT)line, 1, 2, flags, clip);
}

void drawLines(PLINE line, u16 cnt, DrawFlags flags, PRECT clip)
{
	drawLinesImpl((PPOINT)line, cnt, 2, flags, clip);
}

void drawPolyLine(PPOINT points, u16 cnt, DrawFlags flags, PRECT clip)
{
	drawLinesImpl(points, cnt-1, 1, flags, clip);
}

void floodFill(PPOINT pt, COLOUR clr)
{
	// TODO: Implement flood fill algorithm
}

