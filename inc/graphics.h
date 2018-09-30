/*
 * graphics.h
 * High-level graphic routines
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

typedef u8 PIXEL, *PPIXEL;
typedef PIXEL COLOUR;

#define RGBA(r,g,b,a) 		((PIXEL)(a<<6)|(r<<4)|(g<<2)|b)
#define RGB(r,g,b) 			RGBA(r,g,b,3)
#define RED					RGB(3,0,0)
#define GREEN				RGB(0,3,0)
#define BLUE				RGB(0,0,3)
#define YELLOW				RGB(3,3,0)
#define MAGENTA				RGB(3,0,3)
#define CYAN				RGB(0,3,3)
#define WHITE				RGB(3,3,3)
#define BLACK				RGB(0,0,0)
#define GRAY				RGB(2,2,2)
#define DKGRAY				RGB(1,1,1)
#define DKRED				RGB(1,0,0)
#define DKGREEN				RGB(0,1,0)
#define DKBLUE				RGB(0,0,1)

#define ORANGE				RGB(3,1,0)
#define PURPLE				RGB(1,0,1)
#define INDIGO 				RGB(2,0,1)
#define VIOLET				RGB(2,0,3)

#define TRANSPARENT			(PIXEL)0x00
#define SEMITRANSPARENT		(PIXEL)0x80
#define TRANS 				TRANSPARENT
#define STRNS				SEMITRANSPARENT

typedef struct
{
	u16 left;
	u16 top;
} POINT, *PPOINT;

typedef struct
{
	u16 left;
	u16 top;
	u16 width;
	u16 height;
} RECT, *PRECT;

#endif /* GRAPHICS_H_ */
