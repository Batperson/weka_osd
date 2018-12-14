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

void renderHeadingTape(PRENDERER r)
{
	char sz[6];
	LINE lines[LINE_RENDER_BATCH];
	DrawFlags df;
	RECT rc;

	PTAPE pt 		= (PTAPE)r;
	COLOUR ofc		= selectForeColour(pt->hdr.colour);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);

	int vquot;
	float vrem		= remquo(value, pt->unitsPerDivision, &vquot);

	div_t divs		= div(pt->hdr.rect.width, pt->pixelsPerDivision);
	DU offset	  	= nearbyintf((vrem / pt->unitsPerDivision) * pt->pixelsPerDivision);
	DU midPoint 	= pt->hdr.rect.left + (pt->hdr.rect.width >> 1);

	int divsLeft	= divs.quot >> 1;
	DU startPoint	= (midPoint - (divsLeft * pt->pixelsPerDivision)) - offset;

	rc.height 		= pt->font->charheight;

	DU y1, y2, y3;
	if(pt->hdr.flags & RF_ALIGN_BOTTOM)
	{
		y1 = pt->hdr.rect.top + pt->hdr.rect.height;
		y2 = y1 - pt->minorDivisionWidth;
		y3 = y1 - pt->majorDivisionWidth;

		rc.top = pt->hdr.rect.top;
	}
	else
	{
		y1 = pt->hdr.rect.top;
		y2 = y1 + pt->minorDivisionWidth;
		y3 = y1 + pt->majorDivisionWidth;

		rc.top = y3 + 2;
	}

	int majorUnit	= pt->unitsPerDivision * pt->majorDivisionIntervals;
	int currentUnit = (vquot * pt->unitsPerDivision) - (divsLeft * pt->unitsPerDivision);
	if(currentUnit < 0)
		currentUnit += 360;

	while(divs.quot > 0)
	{
		int lcnt = (divs.quot >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs.quot;
		for(int i=0; i<lcnt; i++)
		{
			lines[i].p1.x	= startPoint;
			lines[i].p2.x	= startPoint;
			lines[i].p1.y	= y1;
			lines[i].p2.y	= y2;

			currentUnit %= 360;

			if(currentUnit % majorUnit == 0)
			{
				lines[i].p2.y = y3;

				switch(currentUnit)
				{
				case 0:
					sprintf(sz, "N");
					break;
				case 90:
					sprintf(sz, "E");
					break;
				case 180:
					sprintf(sz, "S");
					break;
				case -90:
				case 270:
					sprintf(sz, "W");
					break;
				default:
					sprintf(sz, "%d", currentUnit);
					break;
				}

				int stringWidth = strlen(sz) * pt->font->charwidth;

				df				= (pt->hdr.flags & RF_ALIGN_BOTTOM) ? AlignBottom : AlignTop;
				if(pt->hdr.flags & RF_OUTLINE)
					df |= Outline;

				rc.left			= startPoint - (stringWidth >> 1);
				rc.width		= stringWidth;

				DU disc;
				if((disc = rc.left - pt->hdr.rect.left) < 0)
				{
					rc.left		-= disc;
					rc.width 	+= disc;
					df			|= AlignRight;
				}
				else if((disc = (rc.left + rc.width) - (pt->hdr.rect.left + pt->hdr.rect.width)) > 0)
				{
					rc.left -= disc;
				}

				drawText(&rc, pt->font, df, sz);
			}

			currentUnit		+= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, None, &pt->hdr.rect);

		divs.quot -= lcnt;
	}

	const DU cx	= 7;
	rc.left 	= midPoint - (cx >> 1);
	rc.top		= (pt->hdr.flags & RF_ALIGN_BOTTOM) ? pt->hdr.rect.top + pt->hdr.rect.height + 2 : pt->hdr.rect.top - cx - 2;
	rc.width	= cx;
	rc.height	= cx;

	drawVertArrow(&rc, (pt->hdr.flags & RF_ALIGN_BOTTOM) ? AlignBottom : AlignTop);

	rc.left		= midPoint - (((pt->font->charwidth) * 3) >> 1);
	rc.top		+= (pt->hdr.flags & RF_ALIGN_BOTTOM) ? cx : -cx;
	rc.width	= (pt->font->charwidth * 3) + 2;
	rc.height	= pt->font->charheight + 4;

	drawRect(&rc, Fill | Outline);

	sprintf(sz, "%d", (int)truncf(value));

	rc.left = midPoint - (((strlen(sz) * pt->font->charwidth) >> 1)-1);
	rc.top += 2;

	drawText(&rc, pt->font, None, sz);

	selectForeColour(ofc);
}

void renderTape(PRENDERER r)
{
	PTAPE pt = (PTAPE)r;

	char sz[6];
	LINE lines[LINE_RENDER_BATCH];
	DrawFlags df;
	RECT rc;

	COLOUR ofc		= selectForeColour(pt->hdr.colour);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);

	int vquot;
	float vrem		= remquo(value, pt->unitsPerDivision, &vquot);

	div_t divs		= div(pt->hdr.rect.height, pt->pixelsPerDivision);
	DU offset	  	= nearbyintf((vrem / pt->unitsPerDivision) * pt->pixelsPerDivision);
	DU midPoint 	= pt->hdr.rect.height >> 1;

	int divsAbove	= divs.quot >> 1;
	DU startPoint	= pt->hdr.rect.top + (midPoint - (divsAbove * pt->pixelsPerDivision)) + offset;

	rc.width 		= pt->hdr.rect.width - (pt->majorDivisionWidth + 2);

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

	int currentUnit = (vquot * pt->unitsPerDivision) + (divsAbove * pt->unitsPerDivision);
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
	DU arrowWidth			= (pt->font->charwidth * 4) + (pt->font->charheight >> 1) + 4;

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

	TAPE tp = { { RF_ALIGN_RIGHT   | RF_OUTLINE, { 338,  0, 20, 288 }, clr, NULL }, offsetof(MODEL, loc.altitude),   1, 20, 5, 4, 2, &systemFont };
	TAPE tp2 = { { RF_ALIGN_LEFT   | RF_OUTLINE, { 1,    0, 20, 288 }, clr, NULL }, offsetof(MODEL, vel.horizontal), 1, 20, 5, 4, 2, &systemFont };
	TAPE tp3 = { { RF_ALIGN_BOTTOM | RF_OUTLINE, { 30, 238, 300, 20 }, clr, NULL }, offsetof(MODEL, att.heading),    2, 10, 5, 4, 2, &systemFont };
	renderTape(&tp.hdr);
	renderTape(&tp2.hdr);
	renderHeadingTape(&tp3.hdr);

	RENDERER ahi = { 0, { 0, 0, 360, 288 }, clr, NULL };
	renderArtificialHorizon(&ahi);

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
