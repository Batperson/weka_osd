/*
 * sprite.h
 * Behaviour and definitions of sprites, which are rectangular screen areas rendered to a line buffer prior to scanout.
 *  Created on: 22/08/2018
 */

#ifndef SPRITE_H_
#define SPRITE_H_

typedef enum
{
	SF_NONE				= 0x00,
	SF_VISIBLE			= 0x01,
	SF_BLINKING			= 0x02,
} SpriteFlagsType;

typedef enum
{
	ST_TESTPATTERN		= 0x00,
	ST_BITMAP			= 0x01,
	ST_LABEL			= 0x02,
} SpriteTypeType;

typedef struct
{
	u16 	flags;
	u16		type;
	Rect 	rect;
} SPRITEHEADER, *PSPRITEHEADER;

typedef SPRITEHEADER SPRITE;
typedef PSPRITEHEADER PSPRITE;

typedef struct
{
	SpriteHeader header;
} BITMAPSPRITE;

#endif /* SPRITE_H_ */
