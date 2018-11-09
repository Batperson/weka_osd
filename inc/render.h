/*
 * render.h
 *
 * Rendering of the OSD
 */

#ifndef RENDER_H_
#define RENDER_H_

typedef struct RENDERER* PRENDERER;
typedef void (*RENDERPROC)(PRENDERER r);

typedef enum {
	RF_NONE								= 0x00,
	RF_HIDDEN							= 0x01,
	RF_BLINK							= 0x02,
	RF_ALIGN_RIGHT						= 0x04
} RenderFlagsType;

typedef struct
{
	u16 flags;
	RECT rect;
	COLOUR colour;
	RENDERPROC renderProc;
} RENDERER;

typedef struct
{
	RENDERER hdr;
	u8 unitsPerDivision;
	u8 pixelsPerDivision;
	u8 majorDivisionIntervals;
	u8 majorDivisionWidth;
	u8 minorDivisionWidth;
} TAPE, *PTAPE;

#endif /* RENDER_H_ */
