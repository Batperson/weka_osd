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
	u16 					flags;
	RECT 					rect;
	void*					renderProc;
} SPRITEHEADER, *PSPRITEHEADER;

typedef SPRITEHEADER SPRITE;
typedef PSPRITEHEADER PSPRITE;

typedef struct
{
	SPRITEHEADER header;
	PIXEL background;
	PIXEL foreground;
	u8 border;
} BOXSPRITE, *PBOXSPRITE;

typedef struct
{
	SPRITEHEADER header;
} BITMAPSPRITE;

PSPRITE initTestpatternSprite(u16 left, u16 top, u16 width, u16 height);

void initSpriteFramework();
void initSprites();

#endif /* SPRITE_H_ */
