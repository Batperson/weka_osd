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
void drawText2(PRECT rect, PFONT font, DrawFlags flags, char* text)
{
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
		y		= (rect->top + rect->height) - font->charheight;
		if(lend < rect->height)
			lstart = lend - rect->height;
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
		if(x + font->charwidth < rect->left)
			continue;
		if(x >= right)
			break;

		// Loop through each char bitmap line
		int yoff = 0;
		for(int i=lstart; i<lend; i++)
		{
			u8* src	= font->data + ((((*c-font->asciiOffset) * font->charheight) + i) * font->bytesPerLine);
			u8* dst = ptToOffset(x, y + yoff++);

			u8 ofst	= ((u32)dst & 0x03);
			dst 	= (u8*)((u32)dst & ~0x03);

			u8 wcnt	= font->charwidth >> 2;
			if(font->charwidth & 0x03)
				wcnt |= 0x01;

			expand2bpp8((u32*)bmap, (u32*)mask, ofst, src, font->charwidth, palette);
			blit((u32*)bmap, (u32*)dst, (u32*)mask, wcnt);
		}

		x += font->charwidth;
	}
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

	sy = 0;
	if(flags & AlignBottom)
	{
		DU d;
		y = (rect->top + rect->height) - font->charheight;
		if((d = y - rect->top) < 0)
			sy = -d;
	}

	COLOUR fg 	= (flags & Inverse) ? background : foreground;
	COLOUR bg 	= (flags & Inverse) ? foreground : background;
	u32 bbrsh 	= bg | bg << 8 | bg << 16 | bg << 24;
	u32 defmask	= (flags & Fill) ? 0xffffffff : 0x00;
	u8 padl		= (font->padding & 0x03);
	u8 padt		= (font->padding & 0x0C) >> 2;
	u8 padr		= (font->padding & 0x30) >> 4;
	u8 padb		= (font->padding & 0xC0) >> 6;

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

void floodFill(PPOINT pt, COLOUR clr)
{
	// TODO: Implement flood fill algorithm
}

