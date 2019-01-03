/*
 * render.h
 *
 * Rendering of the OSD
 */

#ifndef RENDER_H_
#define RENDER_H_

typedef enum {
	RF_NONE								= 0x00,
	RF_ALIGN_LEFT						= 0x00,
	RF_ALIGN_TOP						= 0x00,
	RF_HORIZONTAL						= 0x00,
	RF_ALIGN_RIGHT						= 0x01,
	RF_ALIGN_BOTTOM						= 0x02,
	RF_INVERSE							= 0x60,
	RF_OUTLINE							= 0x80,

	RF_CAPTION							= 0x100,
	RF_VERTICAL							= 0x200,

	RF_BLINK							= 0x4000,
	RF_HIDDEN							= 0x8000,

} RenderFlagsType;

typedef struct RENDERER *PRENDERER;

typedef void (*RENDERPROC)(PRENDERER r);

typedef struct RENDERER
{
	u16 size;
	u16 flags;
	RECT rect;
	COLOUR colour;
	RENDERPROC renderProc;
} RENDERER, *PRENDERER;

typedef struct
{
	RENDERER hdr;
	u16 valueOffset;
} ARROW, *PARROW;

typedef struct
{
	RENDERER hdr;
	u16 pitchValueOffset;
	u16 rollValueOffset;
	u8 unitsPerDivision;
	u8 pixelsPerDivision;
	u8 pitchLadderWidth;
	u8 pitchLadderDirectionHeight;
	u8 horizonLineWidth;
	u8 centreClearance;
	PFONT font;
} AHI, *PAHI;

typedef struct
{
	RENDERER hdr;
	u16 valueOffset;
	float minValue;
	float maxValue;
	u8 unitsPerDivision;
	u8 pixelsPerDivision;
	u8 majorDivisionIntervals;
	u8 majorDivisionWidth;
	u8 minorDivisionWidth;
	PFONT font;
} SCALE, *PSCALE;

typedef struct
{
	float to;
	COLOUR colour;
} SEGMENT, *PSEGMENT;

typedef struct
{
	float min;
	float max;
	u16 cnt;
	PSEGMENT segments;
} RANGE, *PRANGE;

typedef struct
{
	RENDERER hdr;
	u16 valueOffset;
	RANGE range;
	RECT rect2;
	PFONT font;
	char* format;
} INDICATOR, *PINDICATOR;

typedef struct
{
	RENDERER hdr;
	PFONT font;
	char* text;
} LABEL, *PLABEL;

extern PRENDERER* renderers;

#endif /* RENDER_H_ */
