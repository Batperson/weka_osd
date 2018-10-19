/*
 * sprite.h
 * Behaviour and definitions of sprites, which are rectangular screen areas rendered to a line buffer prior to scanout.
 *  Created on: 22/08/2018
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#define MAX_SPRITES 	12

typedef enum
{
	SF_NONE				= 0x00,
	SF_VISIBLE			= 0x01,
	SF_BLINKING			= 0x02,
} SpriteFlagsType;

typedef struct
{
	u16 				flags;
	RECT 				rect;
	COLOUR 				background;
	COLOUR 				foreground;
	void*				renderProc;
} SPRITEHEADER, *PSPRITEHEADER;

typedef SPRITEHEADER SPRITE;
typedef PSPRITEHEADER PSPRITE;

typedef struct
{
	SPRITEHEADER 	hdr;
	u8 				border;
} BOX, *PBOX;

typedef struct
{
	SPRITEHEADER 	hdr;
	u8 				width;
	u8				extent;
} BRACKET, *PBRACKET;

typedef struct
{
	SPRITEHEADER 	hdr;
	PFONT 			font;
	u8				scale;
	u8 				unused1;
	u16				unused2;
	u8* 			text;
} LABEL, *PLABEL;

typedef struct
{
	SPRITEHEADER 	hdr;
	PBITMAP			bitmap;
} BITMAPBOX, *PBITMAPBOX;

PSPRITE newTestpattern(u16 left, u16 top, u16 width, u16 height);
PSPRITE newBox(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, u8 border);
PSPRITE newLabel(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, u8 scale, PFONT font, u8* text);
PSPRITE newBitmapBox(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, COLOUR background, PBITMAP bitmap);
PSPRITE newBracket(u16 left, u16 top, u16 width, u16 height, COLOUR foreground, u8 lineWidth, u8 lineExtent);

void initSpriteFramework();
void initSprites();

#endif /* SPRITE_H_ */
