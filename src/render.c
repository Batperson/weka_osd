/*
 * render.c
 *
 * Rendering of the OSD
 */

#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "video.h"
#include "misc.h"
#include "graphics.h"
#include "system.h"
#include "memory.h"
#include "model.h"
#include "render.h"

#define DEGREE_PIXEL_RATIO 4
#define FBW_H FRAME_BUF_WIDTH / 2
#define FBH_H FRAME_BUF_HEIGHT / 2

void renderArtificialHorizon()
{
	LINE lines[] = {
		{ { FBW_H - 100, FBH_H }, { FBW_H - 20, FBH_H } },
		{ { FBW_H + 100, FBH_H }, { FBW_H + 20, FBH_H } },
		{ { FBW_H - 30, FBH_H - (10 * DEGREE_PIXEL_RATIO) },  { FBW_H + 30, FBH_H - (10 * DEGREE_PIXEL_RATIO) } },
		{ { FBW_H - 30, FBH_H - (20 * DEGREE_PIXEL_RATIO) },  { FBW_H + 30, FBH_H - (20 * DEGREE_PIXEL_RATIO) } },
		{ { FBW_H - 30, FBH_H + (10 * DEGREE_PIXEL_RATIO) },  { FBW_H + 30, FBH_H + (10 * DEGREE_PIXEL_RATIO) } },
		{ { FBW_H - 30, FBH_H + (20 * DEGREE_PIXEL_RATIO) },  { FBW_H + 30, FBH_H + (20 * DEGREE_PIXEL_RATIO) } }
	};

	POINT ctr = { FBW_H, FBH_H };
	rotatePts((PPOINT)lines, sizeof(lines) / sizeof(POINT), &ctr, model.att.roll);
	offsetPts((PPOINT)lines, sizeof(lines) / sizeof(POINT), 0, model.att.pitch);
	drawLines(lines, sizeof(lines) / sizeof(LINE), RGB(0,2,0), NULL);
}

#define LINE_RENDER_BATCH 6

void renderTape(PRENDERER r)
{
	PTAPE pt = (PTAPE)r;

	drawRect(&pt->hdr.rect, SEMITRANSPARENT, SEMITRANSPARENT);

	LINE lines[LINE_RENDER_BATCH];

	float value		= model.loc.altitude;
	DU offset	  	= nearbyintf(fmodf(value, pt->unitsPerDivision) * pt->pixelsPerDivision);

	DU midPoint 	= pt->hdr.rect.height >> 1;
	div_t divs		= div(pt->hdr.rect.height, pt->pixelsPerDivision);
	int divsAbove	= divs.quot >> 1;
	DU startPoint	= (midPoint - (divsAbove * pt->pixelsPerDivision)) + offset;

	DU x1, x2, x3;
	if(pt->hdr.flags & RF_ALIGN_RIGHT)
	{
		x1 = pt->hdr.rect.left;
		x2 = x1 + pt->minorDivisionWidth;
		x3 = x1 + pt->majorDivisionWidth;
	}
	else
	{
		x1 = pt->hdr.rect.left + pt->hdr.rect.width;
		x2 = x1 - pt->minorDivisionWidth;
		x3 = x1 - pt->majorDivisionWidth;
	}

	int currentUnit = ((value / pt->unitsPerDivision) * pt->unitsPerDivision) + (divsAbove * pt->unitsPerDivision);
	int majorUnit	= pt->unitsPerDivision * pt->majorDivisionIntervals;
	while(divs.quot > 0)
	{
		int lcnt = (divs.quot >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs.quot;
		for(int i=0; i<lcnt; i++)
		{
			lines[i].p1.x	= x1;
			lines[i].p2.x	= (currentUnit % majorUnit == 0) ? x3 : x2;
			lines[i].p1.y	= startPoint;
			lines[i].p2.y	= startPoint;

			currentUnit		-= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, pt->hdr.colour, &pt->hdr.rect);

		divs.quot -= lcnt;
	}

	DU arrowHeight			= 11;
	DU arrowWidth			= 25;

	RECT rc;
	rc.left					= (pt->hdr.flags & RF_ALIGN_RIGHT) ? pt->hdr.rect.left : (pt->hdr.rect.left + pt->hdr.rect.width) - arrowWidth;
	rc.top					= midPoint - (arrowHeight >> 1);
	rc.width				= arrowWidth;
	rc.height				= arrowHeight;

	drawArrow(&rc, pt->hdr.colour, SEMITRANSPARENT, (pt->hdr.flags & RF_ALIGN_RIGHT) ? AlignRight : AlignLeft);
}

void INTERRUPT IN_CCM PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	RECT rc = { 20, 10, 120, 20 };
	drawTestPattern(&rc);

	RECT rc2 = { 200, 10, 20, 20 };
	drawRect(&rc2, DKGREEN, TRANSPARENT);

	TAPE tp = { { 0, { 268, 0, 20, 200 }, RGB(0,2,0), NULL }, 1, 20, 5, 8, 4 };
	renderTape(&tp.hdr);

	renderArtificialHorizon();

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
