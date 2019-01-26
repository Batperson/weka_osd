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

#define ORANGE				RGB(3,2,0)
#define LTGREEN 			RGB(2,3,0)
#define AQUAMARINE 			RGB(0,3,2)
#define PURPLE				RGB(1,0,1)
#define INDIGO 				RGB(2,0,1)
#define VIOLET				RGB(2,0,3)

#define TRANSPARENT			(PIXEL)0x00
#define SEMITRANSPARENT		(PIXEL)0x80
#define TRANS 				TRANSPARENT
#define STRNS				SEMITRANSPARENT

typedef s16 DU;

typedef struct
{
	DU x;
	DU y;
} POINT, *PPOINT;

typedef struct
{
	POINT p1;
	POINT p2;
} LINE, *PLINE;

typedef struct
{
	DU left;
	DU top;
	DU width;
	DU height;
} RECT, *PRECT;

typedef struct
{
	u8 	charwidth;
	u8 	charheight;
	u8 	bytesPerLine;
	u8  asciiOffset;
	u8  stride;
	u8* data;
} FONT, *PFONT;

typedef enum
{
	BF_NONE		= 0x00,
	BF_BPP_1	= 0x01,
	BF_BPP_2	= 0x02,
	BF_BPP_4	= 0x03,
	BF_BPP_8	= 0x04
} BitmapFlagsType;

typedef enum
{
	None		= 0x00,
	AlignLeft	= 0x00,
	AlignTop	= 0x00,
	AlignRight	= 0x01,
	AlignBottom	= 0x02,

	Fill		= 0x20,
	Inverse		= 0x40,
	Outline		= 0x80,
} DrawFlags;

typedef struct
{
	u16	width;
	u16 height;
	u16 flags;
	u8* data;
} BITMAP, *PBITMAP;

ALWAYS_INLINE u8 ptInRect(PRECT rc, DU x, DU y) { if(x < rc->left) return 0; if(y < rc->top) return 0; if(x >= rc->left + rc->width) return 0; if(y >= rc->top + rc->height) return 0; return 1; }

void initRect(PRECT rc, DU left, DU top, DU width, DU height);
void inflateRect(PRECT rc, DU width, DU height);
void offsetRect(PRECT rc, DU left, DU top);
void moveRect(PRECT rc, DU left, DU top);
void sizeRect(PRECT rc, DU width, DU height);

void offsetPts(PPOINT ppt, u16 cnt, DU dx, DU dy);
void rotatePts(PPOINT ppt, u16 cnt, PPOINT ctr, float angle);

void clearRenderBuf();

void drawTestPattern(PRECT rect);
void drawRect(PRECT rect, DrawFlags flags);
void drawLine(PLINE line, DrawFlags flags, PRECT clip);
void drawLines(PLINE line, u16 cnt, DrawFlags flags, PRECT clip);
void drawPolyLine(PPOINT points, u16 cnt, DrawFlags flags, PRECT clip);
void drawText(PRECT rect, PFONT font, DrawFlags flags, char* text);
void floodFill(PPOINT pt, COLOUR clr);

void measureText(PFONT font, char* text, PPOINT sz);

COLOUR selectForeColour(COLOUR foreground);
COLOUR selectBackColour(COLOUR background);

#endif /* GRAPHICS_H_ */
