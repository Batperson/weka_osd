/*
 * sprite.c
 * Behaviour and definitions of sprites, which are rectangular screen areas rendered to a line buffer prior to scanout.
 * Created on: 29/09/2018
 */

#include "stm32f4xx.h"
#include "bitband.h"
#include "stdlib.h"
#include "string.h"
#include "misc.h"
#include "memory.h"
#include "graphics.h"
#include "video.h"
#include "system.h"
#include "sprite.h"

extern volatile PPIXEL renderBuf;
extern volatile u16 currentRenderScanLine;
extern FONT systemFont;

PSPRITE sprites[MAX_SPRITES];

static PIXEL	test1[]		= {
		RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED,
		ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE, ORANGE,
		YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
		GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN,
		PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE,
		BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE,
		WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
};
static PIXEL	test2[] 	= {
		RED, RED, RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, RED, RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, RED, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS, STRNS,
		RED, STRNS, STRNS, STRNS, RED, STRNS, STRNS, STRNS, RED, STRNS, STRNS, STRNS,
		RED, STRNS, RED, STRNS, RED, STRNS,RED, STRNS, RED, STRNS, RED,  STRNS
};
static PIXEL	test3[sizeof(test2)];
static PIXEL	test4[sizeof(test2)];

PSPRITE spriteAlloc(size_t size)
{
	// Later will want to implement this ourselves, for now just use stdlib
	return malloc(size);
}

void initSpriteFrameworkTestpattern()
{
	for(u16 i=0; i<sizeof(test2); i++)
	{
		test3[i] = (test2[i] == RED) ? GREEN : STRNS;
		test4[i] = (test2[i] == RED) ? BLUE : STRNS;
	}
}

void renderTestpattern(PSPRITE ps)
{
	const u16 bandHeight = 10;

	void* pdest = renderBuf + ps->rect.left;
	u16 l = currentRenderScanLine - ps->rect.top;
	u16 k = (l / bandHeight) % 4;
	u16 z = (l % bandHeight) ? 1 : 10;

	switch(k * z)
	{
	case 0:
		tilecpy(pdest, test1, ps->rect.width, sizeof(test1));
		break;
	case 1:
		tilecpy(pdest, test2, ps->rect.width, sizeof(test2));
		break;
	case 10:
		memset(pdest, RED, ps->rect.width);
		break;
	case 2:
		tilecpy(pdest, test3, ps->rect.width, sizeof(test3));
		break;
	case 20:
		memset(pdest, GREEN, ps->rect.width);
		break;
	case 3:
		tilecpy(pdest, test4, ps->rect.width, sizeof(test4));
		break;
	case 30:
		memset(pdest, BLUE, ps->rect.width);
		break;
	}
}

void renderBoxBackground(PBOX ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;

	memset(pdest, ps->hdr.background, ps->hdr.rect.width);
}

void renderBox(PBOX ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;
	u16 l = currentRenderScanLine - ps->hdr.rect.top;

	if(l > ps->border && l <= ps->hdr.rect.height - ps->border)
	{
		memset(pdest + ps->border, ps->hdr.background, ps->hdr.rect.width - (ps->border * 2));
		memset(pdest, ps->hdr.foreground, ps->border);
		memset(pdest + (ps->hdr.rect.width - ps->border), ps->hdr.foreground, ps->border);
	}
	else
	{
		memset(pdest, ps->hdr.foreground, ps->hdr.rect.width);
	}
}

void renderBitmapBox(PBITMAPBOX ps)
{

}

void renderBracket(PBRACKET ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;
	u16 l = currentRenderScanLine - ps->hdr.rect.top;

	if(l < ps->width || l >= ps->hdr.rect.height - ps->width)
	{
		byteset(pdest, ps->hdr.foreground, ps->extent);
		byteset((pdest + ps->hdr.rect.width) - ps->extent, ps->hdr.foreground, ps->extent);
	}
	else if(l < ps->extent || l >= ps->hdr.rect.height - ps->extent)
	{
		byteset(pdest, ps->hdr.foreground, ps->width);
		byteset((pdest + ps->hdr.rect.width) - ps->width, ps->hdr.foreground, ps->width);
	}
}

void renderLabel(PLABEL ps)
{
	PFONT pfont 	= ps->font;
	PPIXEL pdest 	= renderBuf + ps->hdr.rect.left;
	PPIXEL pend		= pdest + ps->hdr.rect.width;
	u16 y 			= (currentRenderScanLine - ps->hdr.rect.top) / ps->scale;

	if(ps->hdr.background != TRANSPARENT)
		memset(pdest, ps->hdr.background, ps->hdr.rect.width);

	if(y <= pfont->charheight)
	{
		int cx		= pfont->charwidth * ps->scale;

		for(u8* p = ps->text; *p != 0; p++)
		{
			u8* psrc  	= pfont->data + ((*p-32) * pfont->charheight * pfont->charbytes) + y;
			for(int j=0; j<cx; j++)
			{
				if(*BITBAND_PTR(psrc, 7-(j / ps->scale)))
					*pdest = ps->hdr.foreground;
				if(++pdest >= pend)
					return;
			}
		}
	}
}

PSPRITE initSpriteHeader(PSPRITE ps, u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, u16 flags)
{
	ps->flags 		= flags;
	ps->rect.left 	= left;
	ps->rect.top	= top;
	ps->rect.width	= width;
	ps->rect.height	= height;
	ps->foreground	= foreground;
	ps->background	= background;

	return ps;
}

PSPRITE newTestpattern(u16 left, u16 top, u16 width, u16 height)
{
	PSPRITE ps 		= initSpriteHeader(spriteAlloc(sizeof(SPRITE)), left, top, width, height, TRANSPARENT, TRANSPARENT, SF_VISIBLE);
	ps->renderProc	= renderTestpattern;

	return ps;
}

PSPRITE newBox(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, u8 border)
{
	PBOX ps 				= (PBOX)initSpriteHeader(spriteAlloc(sizeof(BOX)), left, top, width, height, foreground, background, SF_VISIBLE);
	ps->hdr.renderProc		= (border > 0 && foreground != background) ? renderBox : renderBoxBackground;
	ps->border				= border;

	return (PSPRITE)ps;
}

PSPRITE newBracket(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, u8 lineWidth, u8 lineExtent)
{
	PBRACKET ps 			= (PBRACKET)initSpriteHeader(spriteAlloc(sizeof(BRACKET)), left, top, width, height, foreground, TRANSPARENT, SF_VISIBLE);
	ps->hdr.renderProc		= renderBracket;
	ps->width				= lineWidth;
	ps->extent				= lineExtent;

	return (PSPRITE)ps;
}

PSPRITE newBitmapBox(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, PBITMAP bitmap)
{
	PBITMAPBOX ps 			= (PBITMAPBOX)initSpriteHeader(spriteAlloc(sizeof(BITMAPBOX)), left, top, width, height, foreground, background, SF_VISIBLE);
	ps->hdr.renderProc		= renderBitmapBox;
	ps->bitmap				= bitmap;

	return (PSPRITE)ps;
}

PSPRITE newLabel(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, u8 scale, PFONT font, u8* text)
{
	PLABEL ps 				= (PLABEL)initSpriteHeader(spriteAlloc(sizeof(LABEL)), left, top, width, height, foreground, background, SF_VISIBLE);
	ps->hdr.renderProc		= renderLabel;
	ps->scale				= scale;
	ps->xoffset				= 0;
	ps->yoffset				= 0;
	ps->text				= text;
	ps->font				= font;

	return (PSPRITE)ps;
}

// One-time startup initialization here
void initSpriteFramework()
{
	initSpriteFrameworkTestpattern();
}

// Instance initialization here. In the future this will come from persisted state.
u8 weka[] = { "WEKA OSD!" };	// System font is incomplete, most lower case is missing
void initSprites()
{
	memset(sprites, 0, sizeof(sprites));

	sprites[0] 	= newTestpattern(60, 60, 100, 100);
	sprites[1] 	= newBracket(200, 200, 20, 20, YELLOW, 1, 6);
	sprites[2] 	= newLabel(180, 120, 120, 50, YELLOW, TRANSPARENT, 2, &systemFont, weka);

	sprites[2]->flags |= SF_BLINKING;
}


