/*
 * sprite.c
 * Behaviour and definitions of sprites, which are rectangular screen areas rendered to a line buffer prior to scanout.
 * Created on: 29/09/2018
 */

#include "stm32f4xx.h"
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
	void* pdest = renderBuf + ps->rect.left;
	u16 l = currentRenderScanLine - ps->rect.top;
	u16 k = (l / 10) % 4;

	switch(k)
	{
	case 0:
		tilecpy(pdest, test1, ps->rect.width, sizeof(test1));
		break;
	case 1:
		tilecpy(pdest, test2, ps->rect.width, sizeof(test2));
		break;
	case 2:
		tilecpy(pdest, test3, ps->rect.width, sizeof(test3));
		break;
	case 3:
		tilecpy(pdest, test4, ps->rect.width, sizeof(test4));
		break;
	}
}

void renderBoxBackground(PBOX ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;

	memset(pdest, ps->background, ps->hdr.rect.width);
}

void renderBox(PBOX ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;
	u16 l = currentRenderScanLine - ps->hdr.rect.top;

	if(l > ps->border && l <= ps->hdr.rect.height - ps->border)
	{
		memset(pdest + ps->border, ps->background, ps->hdr.rect.width - (ps->border * 2));
		memset(pdest, ps->foreground, ps->border);
		memset(pdest + (ps->hdr.rect.width - ps->border), ps->foreground, ps->border);
	}
	else
	{
		memset(pdest, ps->foreground, ps->hdr.rect.width);
	}
}

void renderLabel(PLABEL ps)
{
	void* pdest = renderBuf + ps->hdr.rect.left;
	u16 l = currentRenderScanLine - ps->hdr.rect.top;
}

PSPRITE initTestpattern(u16 left, u16 top, u16 width, u16 height)
{
	PSPRITE ps 		= spriteAlloc(sizeof(SPRITE));

	ps->flags 		= SF_VISIBLE;
	ps->rect.left 	= left;
	ps->rect.top	= top;
	ps->rect.width	= width;
	ps->rect.height	= height;
	ps->renderProc	= renderTestpattern;

	return ps;
}

PSPRITE initBox(u16 left, u16 top, u16 width, u16 height, PIXEL foreground, PIXEL background, u8 border)
{
	PBOX ps 				= (PBOX)spriteAlloc(sizeof(BOX));
	ps->hdr.flags 			= SF_VISIBLE;
	ps->hdr.rect.left 		= left;
	ps->hdr.rect.top		= top;
	ps->hdr.rect.width		= width;
	ps->hdr.rect.height		= height;
	ps->hdr.renderProc		= (border > 0 && foreground != background) ? renderBox : renderBoxBackground;
	ps->foreground			= foreground;
	ps->background			= background;
	ps->border				= border;

	return (PSPRITE)ps;
}

PSPRITE initLabel(u16 left, u16 top, u16 width, u16 height, PIXEL foreground, PIXEL background, char* sz)
{
	PLABEL ps 				= (PLABEL)spriteAlloc(sizeof(LABEL));
	ps->hdr.flags 			= SF_VISIBLE;
	ps->hdr.rect.left 		= left;
	ps->hdr.rect.top		= top;
	ps->hdr.rect.width		= width;
	ps->hdr.rect.height		= height;
	ps->hdr.renderProc		= renderLabel;
	ps->foreground			= foreground;
	ps->background			= background;
	ps->sz					= sz;

	return (PSPRITE)ps;
}

// One-time startup initialization here
void initSpriteFramework()
{
	initSpriteFrameworkTestpattern();
}

// Instance initialization here. In the future this will come from persisted state.
void initSprites()
{
	memset(sprites, 0, sizeof(sprites));

	sprites[0] 	= initTestpattern(60, 60, 100, 100);
}


