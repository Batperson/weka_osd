/*
 * render.c
 *
 * Rendering of the OSD
 */

#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "video.h"
#include "misc.h"
#include "graphics.h"
#include "system.h"
#include "memory.h"
#include "model.h"
#include "render.h"

#define DEGREE_PIXEL_RATIO 4

extern FONT systemFont;

void renderArtificialHorizon(PRENDERER r)
{
	DU hw = r->rect.width / 2;
	DU hh = r->rect.height / 2;

	LINE lines[] = {
		{ { hw - 100, hh }, { hw - 20, hh } },
		{ { hw + 100, hh }, { hw + 20, hh } },
		{ { hw - 30, hh - (10 * DEGREE_PIXEL_RATIO) },  { hw + 30, hh - (10 * DEGREE_PIXEL_RATIO) } },
		{ { hw - 30, hh - (20 * DEGREE_PIXEL_RATIO) },  { hw + 30, hh - (20 * DEGREE_PIXEL_RATIO) } },
		{ { hw - 30, hh + (10 * DEGREE_PIXEL_RATIO) },  { hw + 30, hh + (10 * DEGREE_PIXEL_RATIO) } },
		{ { hw - 30, hh + (20 * DEGREE_PIXEL_RATIO) },  { hw + 30, hh + (20 * DEGREE_PIXEL_RATIO) } }
	};

	POINT ctr = { r->rect.left + hw, r->rect.top + hh };
	rotatePts((PPOINT)lines, sizeof(lines) / sizeof(POINT), &ctr, model.att.roll);
	offsetPts((PPOINT)lines, sizeof(lines) / sizeof(POINT), 0, model.att.pitch);
	drawLines(lines, sizeof(lines) / sizeof(LINE), r->colour, NULL);
}

#define LINE_RENDER_BATCH 6


void renderTape(PRENDERER r)
{
	PTAPE pt = (PTAPE)r;

	drawRect(&pt->hdr.rect, SEMITRANSPARENT, SEMITRANSPARENT);

	char sz[6];
	LINE lines[LINE_RENDER_BATCH];

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);
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

	DU arrowHeight			= 13;
	DU arrowWidth			= 30;

	RECT rc;
	rc.left					= (pt->hdr.flags & RF_ALIGN_RIGHT) ? pt->hdr.rect.left : (pt->hdr.rect.left + pt->hdr.rect.width) - arrowWidth;
	rc.top					= midPoint - (arrowHeight >> 1);
	rc.width				= arrowWidth;
	rc.height				= arrowHeight;

	drawArrow(&rc, pt->hdr.colour, SEMITRANSPARENT, (pt->hdr.flags & RF_ALIGN_RIGHT) ? AlignRight : AlignLeft);

	rc.top += 2;
	rc.width -= arrowHeight >> 1;

	sprintf(sz, "%d", (int)truncf(value));
	if(pt->hdr.flags & RF_ALIGN_RIGHT)
	{
		rc.left += (arrowHeight >> 1);
		drawText(&rc, &systemFont, pt->hdr.colour, AlignLeft, sz);
	}
	else
	{
		rc.left += 1;
		drawText(&rc, &systemFont, pt->hdr.colour, AlignRight, sz);
	}
}

void INTERRUPT IN_CCM PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	RECT rc = { 30, 10, 120, 20 };
	drawTestPattern(&rc);

	if(blinkOn())
	{
		RECT rc2 = { 200, 170, 60, 20 };
		drawText(&rc2, &systemFont, RED, AlignLeft, "ARMED");
	}

	TAPE tp = { { RF_NONE,         { 268, 0, 20, 200 }, RGB(0,2,0), NULL }, offsetof(MODEL, loc.altitude), 1, 20, 5, 8, 4 };
	TAPE tp2 = { { RF_ALIGN_RIGHT, { 1, 0, 20, 200 }, RGB(0,2,0), NULL }, offsetof(MODEL, vel.horizontal), 1, 20, 5, 8, 4 };
	renderTape(&tp.hdr);
	renderTape(&tp2.hdr);

	RENDERER ahi = { 0, { 0, 0, 300, 200 }, RGB(0,2,0), NULL };
	renderArtificialHorizon(&ahi);

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
