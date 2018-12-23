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
extern char szBtn0Msg[];
extern char szBtn1Msg[];

void renderArtificialHorizon(PRENDERER r)
{
	COLOUR ofc		= selectForeColour(r->colour);

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

	offsetPts((PPOINT)lines, sizeof(lines) / sizeof(POINT), +1, +1);
	drawLines(lines, sizeof(lines) / sizeof(LINE), None, NULL);

	selectForeColour(ofc);
}

#define LINE_RENDER_BATCH 6


void renderTape(PRENDERER r)
{
	PTAPE pt = (PTAPE)r;

	char sz[6];
	LINE lines[LINE_RENDER_BATCH];
	DrawFlags df;
	RECT rc;

	COLOUR ofc		= selectForeColour(pt->hdr.colour);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);
	DU offset	  	= nearbyintf(fmodf(value, pt->unitsPerDivision) * pt->pixelsPerDivision);

	DU midPoint 	= pt->hdr.rect.height >> 1;
	div_t divs		= div(pt->hdr.rect.height, pt->pixelsPerDivision);
	int divsAbove	= divs.quot >> 1;
	DU startPoint	= (midPoint - (divsAbove * pt->pixelsPerDivision)) + offset;

	rc.width 	= pt->hdr.rect.width - (pt->majorDivisionWidth + 2);

	DU x1, x2, x3;
	if(pt->hdr.flags & RF_ALIGN_RIGHT)
	{
		x1 = pt->hdr.rect.left + pt->hdr.rect.width;
		x2 = x1 - pt->minorDivisionWidth;
		x3 = x1 - pt->majorDivisionWidth;

		rc.left = pt->hdr.rect.left;
	}
	else
	{
		x1 = pt->hdr.rect.left;
		x2 = x1 + pt->minorDivisionWidth;
		x3 = x1 + pt->majorDivisionWidth;

		rc.left = x3 + 2;
	}

	int currentUnit = ((value / pt->unitsPerDivision) * pt->unitsPerDivision) + (divsAbove * pt->unitsPerDivision);
	int majorUnit	= pt->unitsPerDivision * pt->majorDivisionIntervals;
	while(divs.quot > 0)
	{
		int lcnt = (divs.quot >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs.quot;
		for(int i=0; i<lcnt; i++)
		{
			lines[i].p1.x	= x1;
			lines[i].p2.x	= x2;
			lines[i].p1.y	= startPoint;
			lines[i].p2.y	= startPoint;

			if(currentUnit % majorUnit == 0)
			{
				lines[i].p2.x = x3;

				df				= (pt->hdr.flags & RF_ALIGN_RIGHT) ? AlignRight : AlignLeft;
				if(pt->hdr.flags & RF_OUTLINE)
					df |= Outline;

				rc.top			= startPoint - (pt->font->charheight >> 1);
				rc.height		= pt->font->charheight;

				DU disc;
				if((disc = rc.top - pt->hdr.rect.top) < 0)
				{
					rc.top 		-= disc;
					rc.height 	+= disc;
					df			|= AlignBottom;
				}
				else if((disc = (rc.top + rc.height) - (pt->hdr.rect.top + pt->hdr.rect.height)) > 0)
				{
					rc.height -= disc;
				}

				sprintf(sz, "%d", currentUnit);

				drawText(&rc, pt->font, df, sz);
			}

			currentUnit		-= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, None, &pt->hdr.rect);

		divs.quot -= lcnt;
	}

	DU arrowHeight			= pt->font->charheight + 4;
	DU arrowWidth			= 30;

	rc.left					= (pt->hdr.flags & RF_ALIGN_RIGHT) ? (pt->hdr.rect.left + pt->hdr.rect.width) - arrowWidth : pt->hdr.rect.left;
	rc.top					= midPoint - (arrowHeight >> 1);
	rc.width				= arrowWidth;
	rc.height				= arrowHeight;

	drawArrow(&rc, (pt->hdr.flags & RF_ALIGN_RIGHT) ? AlignLeft : AlignRight );

	rc.top += 2;
	rc.left += (pt->hdr.flags & RF_ALIGN_RIGHT) ? 1 : arrowHeight >> 1;
	rc.width -= arrowHeight >> 1;

	sprintf(sz, "%d", (int)truncf(value));

	drawText(&rc, pt->font, df, sz);

	selectForeColour(ofc);
}

void INTERRUPT PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	COLOUR clr =  RGB(2,3,2);


	RECT rc = { 1, 20, 100, 20 };
	drawTestPattern(&rc);

	RECT rc2 = { 300, 240, 60, 20 };
	if(blinkOn())
	{
		selectForeColour(RGB(3,3,0));
		drawText(&rc2, &systemFont, AlignLeft | Inverse, "ARMED");
		selectForeColour(RGB(3,3,3));
	}

	rc2.top = 160;
	drawText(&rc2, &systemFont, AlignLeft | Inverse, szBtn0Msg);

	rc2.top = 180;
	drawText(&rc2, &systemFont, AlignLeft, szBtn1Msg);

	TAPE tp = { { RF_ALIGN_RIGHT | RF_OUTLINE, { 338, 0, 20, 288 }, clr, NULL }, offsetof(MODEL, loc.altitude),   1, 20, 5, 4, 2, &systemFont };
	TAPE tp2 = { { RF_NONE | RF_OUTLINE,       { 1,   0, 20, 288 }, clr, NULL }, offsetof(MODEL, vel.horizontal), 1, 20, 5, 4, 2, &systemFont };
	renderTape(&tp.hdr);
	renderTape(&tp2.hdr);

	RENDERER ahi = { 0, { 0, 0, 360, 288 }, clr, NULL };
	renderArtificialHorizon(&ahi);

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
