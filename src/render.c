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

#define LINE_RENDER_BATCH 6

extern FONT systemFont;
extern FONT tinyFont;
extern char szBtn0Msg[];
extern char szBtn1Msg[];

PSEGMENT getSegment(PRANGE range, float value)
{
	PSEGMENT seg = range->segments;
	int i = 0;

	for(; i<range->cnt-1; i++)
	{
		if(value < range->segments[i].to)
		{
			seg = &range->segments[i];
			break;
		}
	}

	if(i == range->cnt-1)
		seg = &range->segments[i];

	return seg;
}

void renderBarFill(PRECT rc, PSEGMENT seg, float scale, RenderFlagsType flags)
{
	COLOUR obc		= selectBackColour(seg->colour);

	RECT r = { rc->left, rc->top };
	if(flags & RF_VERTICAL)
	{
		r.width		= rc->width;
		r.height	= rc->height * scale;
		if(r.height <= 0)
			r.height = 1;
	}
	else
	{
		r.height	= rc->height;
		r.width		= rc->width * scale;
		if(r.width <= 0)
			r.width = 1;
	}

	drawRect(&r, Fill);

	selectBackColour(obc);
}

void renderBarMeter(PRENDERER r)
{
	PINDICATOR pi = (PINDICATOR)r;

	float value		= DEREFERENCE_OFFSET_FLOAT(pi->valueOffset);
	float scale		= (value - pi->range.min) / (pi->range.max - pi->range.min);
	if(scale > 1)
		scale = 1;
	else if(scale <= 0)
		scale = 0;

	PSEGMENT seg	= getSegment(&pi->range, value);

	COLOUR ofc		= selectForeColour(r->colour);

	RECT rc			= { r->rect.left + 2, r->rect.top + 2, r->rect.width - 4, r->rect.height - 4 };

	drawRect(&r->rect, Outline);

	renderBarFill(&rc, seg, scale, r->flags);

	if(r->flags & RF_CAPTION)
	{
		char sz[8];
		snprintf(sz, sizeof(sz), pi->format, value);

		drawText(&pi->rect2, pi->font, r->flags & RF_OUTLINE, sz);
	}

	selectForeColour(ofc);
}

void renderBatteryMeter(PRENDERER r)
{
	PINDICATOR pi = (PINDICATOR)r;

	float value		= DEREFERENCE_OFFSET_FLOAT(pi->valueOffset);
	float scale		= (value - pi->range.min) / (pi->range.max - pi->range.min);
	if(scale > 1)
		scale = 1;
	else if(scale <= 0)
		scale = 0;

	PSEGMENT seg	= getSegment(&pi->range, value);

	DU n			= r->rect.height / 3;

	COLOUR ofc		= selectForeColour(r->colour);

	POINT pt[9];
	RECT rc;

	if(r->flags & RF_ALIGN_RIGHT)
	{
		pt[0].x		= r->rect.left;
		pt[0].y		= r->rect.top;
		pt[1].x		= r->rect.left + r->rect.width - 1;
		pt[1].y		= r->rect.top;
		pt[2].x		= pt[1].x;
		pt[2].y		= r->rect.top + n;
		pt[3].x		= pt[2].x + 1;
		pt[3].y		= r->rect.top + n;
		pt[4].x		= pt[3].x;
		pt[4].y		= r->rect.top + r->rect.height - n;
		pt[5].x		= pt[4].x - 1;
		pt[5].y		= r->rect.top + r->rect.height - n;
		pt[6].x		= pt[5].x;
		pt[6].y		= r->rect.top + r->rect.height;
		pt[7].x		= r->rect.left;
		pt[7].y		= r->rect.top + r->rect.height;
		pt[8].x		= r->rect.left;
		pt[8].y		= r->rect.top;

		rc.left		= r->rect.left + 2;
		rc.top		= r->rect.top + 2;
		rc.height	= r->rect.height - 3;
		rc.width	= r->rect.width - 4;
	}
	else
	{
		pt[0].x		= r->rect.left + 1;
		pt[0].y		= r->rect.top;
		pt[1].x		= pt[0].x;
		pt[1].y		= r->rect.top + n;
		pt[2].x		= r->rect.left;
		pt[2].y		= r->rect.top + n;
		pt[3].x		= pt[2].x;
		pt[3].y		= r->rect.top + (r->rect.height - n);
		pt[4].x		= pt[3].x + 1;
		pt[4].y		= r->rect.top + (r->rect.height - n);
		pt[5].x		= pt[4].x;
		pt[5].y		= r->rect.top + r->rect.height;
		pt[6].x		= r->rect.left + r->rect.width;
		pt[6].y		= r->rect.top + r->rect.height;
		pt[7].x		= r->rect.left + r->rect.width;
		pt[7].y		= r->rect.top;
		pt[8].x		= r->rect.left + 1;
		pt[8].y		= r->rect.top;

		rc.left		= r->rect.left + 3;
		rc.top		= r->rect.top + 2;
		rc.height	= r->rect.height - 4;
		rc.width	= r->rect.width - 4;
	}

	drawPolyLine(pt, sizeof(pt) / sizeof(POINT), None, NULL);

	renderBarFill(&rc, seg, scale, r->flags);

	if(r->flags & RF_CAPTION)
	{
		char sz[8];
		snprintf(sz, sizeof(sz), pi->format, value);

		drawText(&pi->rect2, pi->font, r->flags & RF_OUTLINE, sz);
	}

	selectForeColour(ofc);
}

void renderArtificialHorizon(PRENDERER r)
{
	PAHI pa = (PAHI)r;
	COLOUR ofc		= selectForeColour(r->colour);

	POINT ctr = {
			r->rect.left + (r->rect.width >> 1),
			r->rect.top + (r->rect.height >> 1) };

	LINE lines[LINE_RENDER_BATCH * 4];

	float pitchValue = DEREFERENCE_OFFSET_FLOAT(pa->pitchValueOffset);
	float rollValue = DEREFERENCE_OFFSET_FLOAT(pa->rollValueOffset);

	int vquot;
	int divs		= pa->hdr.rect.height / pa->pixelsPerDivision;
	float vrem		= remquo(pitchValue, pa->unitsPerDivision, &vquot);

	DU offset	  	= nearbyintf((vrem / pa->unitsPerDivision) * pa->pixelsPerDivision);

	int divsAbove	= divs >> 1;
	DU currentPos	= (ctr.y - (divsAbove * pa->pixelsPerDivision)) - offset;

	int currentUnit = (vquot * pa->unitsPerDivision) - (divsAbove * pa->unitsPerDivision);

	while(divs > 0)
	{
		int lcnt = 0;
		int cnt = (divs >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs;
		for(int i=0; i<cnt; i++)
		{
			int rungIncr = (currentUnit > 0) ? -pa->pitchLadderDirectionHeight : pa->pitchLadderDirectionHeight;
			if(currentUnit == 0)
			{
				lines[lcnt].p1.x 	= ctr.x - pa->centreClearance - pa->horizonLineWidth;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x 	= ctr.x - pa->centreClearance;
				lines[lcnt++].p2.y 	= currentPos;
				lines[lcnt].p1.x 	= ctr.x + pa->centreClearance;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x 	= ctr.x + pa->centreClearance + pa->horizonLineWidth;
				lines[lcnt++].p2.y 	= currentPos;
			}
			else
			{
				lines[lcnt].p1.x 	= ctr.x - pa->centreClearance - pa->pitchLadderWidth;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x 	= ctr.x - pa->centreClearance;
				lines[lcnt++].p2.y 	= currentPos;
				lines[lcnt].p1.x	= ctr.x - pa->centreClearance;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x	= ctr.x - pa->centreClearance;
				lines[lcnt++].p2.y 	= currentPos + rungIncr;
				lines[lcnt].p1.x 	= ctr.x + pa->centreClearance;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x 	= ctr.x + pa->centreClearance + pa->pitchLadderWidth;
				lines[lcnt++].p2.y 	= currentPos;
				lines[lcnt].p1.x	= ctr.x + pa->centreClearance;
				lines[lcnt].p1.y 	= currentPos;
				lines[lcnt].p2.x	= ctr.x + pa->centreClearance;
				lines[lcnt++].p2.y 	= currentPos + rungIncr;
			}

			currentUnit		+= pa->unitsPerDivision;
			currentPos		+= pa->pixelsPerDivision;
		}

		rotatePts((PPOINT)lines, lcnt << 1, &ctr, rollValue);
		drawLines(lines, lcnt, None, &pa->hdr.rect);

		divs -= cnt;
	}

	selectForeColour(ofc);
}

void renderArrow(PRENDERER r)
{
	PARROW pa = (PARROW)r;
	DU midX	= r->rect.left + (r->rect.width >> 1);
	DU midY	= r->rect.top + (r->rect.height >> 1);
	DU btmY = r->rect.top + (r->rect.height - (r->rect.height / 3));

	POINT ctr = { midX, midY };
	POINT pts[5] = {
			{ midX, r->rect.top },
			{ r->rect.left, r->rect.top + r->rect.height },
			{ midX, btmY },
			{ r->rect.left + r->rect.width,  r->rect.top + r->rect.height },
			{ midX, r->rect.top }
	};

	float value		= DEREFERENCE_OFFSET_FLOAT(pa->valueOffset);

	COLOUR ofc		= selectForeColour(r->colour);

	rotatePts(pts, sizeof(pts) / sizeof(POINT), &ctr, value);
	drawPolyLine(pts, sizeof(pts) / sizeof(POINT), None, NULL);
	floodFill(&ctr, r->colour);

	selectForeColour(ofc);
}

void renderHeadingTape(PRENDERER r)
{
	char sz[6];
	LINE lines[LINE_RENDER_BATCH];

	RECT rc;

	PTAPE pt 		= (PTAPE)r;
	DrawFlags df	= pt->hdr.flags & (RF_ALIGN_BOTTOM | RF_OUTLINE);
	COLOUR ofc		= selectForeColour(pt->hdr.colour);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);

	int vquot;
	int divs		= pt->hdr.rect.width / pt->pixelsPerDivision;
	float vrem		= remquo(value, pt->unitsPerDivision, &vquot);

	DU offset	  	= nearbyintf((vrem / pt->unitsPerDivision) * pt->pixelsPerDivision);
	DU midPoint 	= pt->hdr.rect.left + (pt->hdr.rect.width >> 1);

	int divsLeft	= divs >> 1;
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

	while(divs > 0)
	{
		int lcnt = (divs >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs;
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

				DrawFlags df2	= None;

				rc.left			= startPoint - (stringWidth >> 1);
				rc.width		= stringWidth;

				DU disc;
				if((disc = rc.left - pt->hdr.rect.left) < 0)
				{
					rc.left		-= disc;
					rc.width 	+= disc;
					df2			= AlignRight;
				}
				else if((disc = (rc.left + rc.width) - (pt->hdr.rect.left + pt->hdr.rect.width)) > 0)
				{
					rc.left -= disc;
				}

				drawText(&rc, pt->font, df | df2, sz);
			}

			currentUnit		+= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, None, &pt->hdr.rect);

		divs -= lcnt;
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
	RECT rc;

	COLOUR ofc		= selectForeColour(pt->hdr.colour);
	DrawFlags df	= pt->hdr.flags & (RF_ALIGN_RIGHT | RF_OUTLINE);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);

	int vquot;
	int divs		= pt->hdr.rect.height / pt->pixelsPerDivision;
	float vrem		= remquo(value, pt->unitsPerDivision, &vquot);

	DU offset	  	= nearbyintf((vrem / pt->unitsPerDivision) * pt->pixelsPerDivision);
	DU midPoint 	= pt->hdr.rect.height >> 1;

	int divsAbove	= divs >> 1;
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
	while(divs > 0)
	{
		int lcnt = (divs >= LINE_RENDER_BATCH) ? LINE_RENDER_BATCH : divs;
		for(int i=0; i<lcnt; i++)
		{
			lines[i].p1.x	= x1;
			lines[i].p2.x	= x2;
			lines[i].p1.y	= startPoint;
			lines[i].p2.y	= startPoint;

			if(currentUnit % majorUnit == 0)
			{
				lines[i].p2.x = x3;

				DrawFlags df2	= None;

				rc.top			= startPoint - (pt->font->charheight >> 1);
				rc.height		= pt->font->charheight;

				DU disc;
				if((disc = rc.top - pt->hdr.rect.top) < 0)
				{
					rc.top 		-= disc;
					rc.height 	+= disc;
					df2			= AlignBottom;
				}
				else if((disc = (rc.top + rc.height) - (pt->hdr.rect.top + pt->hdr.rect.height)) > 0)
				{
					rc.height -= disc;
				}

				sprintf(sz, "%d", currentUnit);

				drawText(&rc, pt->font, df | df2, sz);
			}

			currentUnit		-= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, None, &pt->hdr.rect);

		divs -= lcnt;
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

	TAPE tp = { { RF_ALIGN_RIGHT   | RF_OUTLINE, { 338,  0, 20, 288 }, clr, NULL }, offsetof(MODEL, loc.altitude),   1, 20, 5, 4, 2, &systemFont };
	TAPE tp2 = { { RF_ALIGN_LEFT   | RF_OUTLINE, { 1,    0, 20, 288 }, clr, NULL }, offsetof(MODEL, vel.horizontal), 1, 20, 5, 4, 2, &systemFont };
	TAPE tp3 = { { RF_ALIGN_BOTTOM | RF_OUTLINE, { 30, 238, 300, 20 }, clr, NULL }, offsetof(MODEL, att.heading),    2, 10, 5, 4, 2, &systemFont };
	renderTape(&tp.hdr);
	renderTape(&tp2.hdr);
	renderHeadingTape(&tp3.hdr);

	ARROW arrow = { { RF_OUTLINE, { 300,  50, 10, 20 }, clr, NULL }, offsetof(MODEL, att.homeVector) };
	renderArrow(&arrow.hdr);

	AHI ahi = { { 0, { 0, 0, 360, 288 }, clr, NULL },
			offsetof(MODEL, att.pitch),
			offsetof(MODEL, att.roll),
			10, 60,
			30, 5,
			80, 20 };

	SEGMENT segs[] = { { 9.6f, RED }, { 10.0f, ORANGE }, { 10.8f, YELLOW }, { 12.6f, GREEN  } };
	INDICATOR battMeter = { { RF_ALIGN_LEFT | RF_CAPTION | RF_OUTLINE, { 30, 20, 25, 12 }, clr, NULL },
			offsetof(MODEL, elec.voltage),
			{ 9.0f, 12.6f, sizeof(segs) / sizeof(SEGMENT), segs },
			{ 56, 22, 40, 12},
			&systemFont,
			"%.1fV" };

	SEGMENT segs2[] = { { 40.0f, GREEN }, { 50.0f, YELLOW }, { 60.0f, RED  } };
	INDICATOR ampMeter = { { RF_ALIGN_LEFT | RF_CAPTION | RF_OUTLINE, { 30, 34, 25, 12 }, clr, NULL },
			offsetof(MODEL, elec.current),
			{ 0.0f, 60.0f, sizeof(segs2) / sizeof(SEGMENT), segs2 },
			{ 56, 36, 40, 12},
			&systemFont,
			"%.1fA" };

	renderBatteryMeter(&battMeter.hdr);
	renderBarMeter(&ampMeter.hdr);

	renderArtificialHorizon(&ahi.hdr);

	RECT rc2 = { 300, 265, 60, 20 };
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

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
