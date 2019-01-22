/*
 * render.c
 *
 * Renderer implementation
 * High-level routines which draw the OSD components to the frame buffer
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


extern char szBtn0Msg[];
extern char szBtn1Msg[];

PRENDERER* renderers;

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

	if(flags & RF_VERTICAL)
	{
		rc->height	*= scale;
		if(rc->height < 1)
			rc->height = 1;
	}
	else
	{
		rc->width *= scale;
		if(rc->width < 1)
			rc->width = 1;
	}

	drawRect(rc, Fill);

	selectBackColour(obc);
}

void renderLabel(PRENDERER r)
{
	PLABEL pl = (PLABEL)r;
	COLOUR ofc		= selectForeColour(r->colour);

	if(pl->text)
	{
		drawText(&pl->hdr.rect, pl->font, pl->hdr.flags & (RF_ALIGN_TOP | RF_ALIGN_RIGHT | RF_INVERSE | RF_OUTLINE), pl->text);
	}

	selectForeColour(ofc);
}

void renderDynLabel(PRENDERER r)
{
	PDYNLABEL pl = (PDYNLABEL)r;
	COLOUR ofc		= selectForeColour(r->colour);

	if(pl->format)
	{
		char sz[20];
		float value 	= DEREFERENCE_OFFSET_FLOAT(pl->valueOffset);

		snprintf(sz, sizeof(sz), pl->format, value);
		drawText(&pl->hdr.rect, pl->font, pl->hdr.flags & (RF_ALIGN_TOP | RF_ALIGN_RIGHT | RF_INVERSE | RF_OUTLINE), sz);
	}

	selectForeColour(ofc);
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

	RECT rc			= { r->rect.left, r->rect.top, r->rect.width, r->rect.height };

	COLOUR ofc		= selectForeColour(r->colour);

	if(r->flags & RF_OUTLINE)
	{
		drawRect(&rc, Outline | Inverse);
		inflateRect(&rc, -1, -1);
	}

	drawRect(&rc, Outline);

	PSEGMENT seg	= getSegment(&pi->range, value);

	selectForeColour(r->colour);
	inflateRect(&rc, -2, -2);
	renderBarFill(&rc, seg, scale, r->flags);

	selectForeColour(ofc);
}

void initBatteryMeterPolygon(POINT pt[9], RenderFlagsType rf)
{

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

	if(r->flags & RF_OUTLINE)
		inflateRect(&rc, -1, -1);

	drawPolyLine(pt, sizeof(pt) / sizeof(POINT), r->flags & RF_OUTLINE, NULL);

	renderBarFill(&rc, seg, scale, r->flags);

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
			int incr = (currentUnit > 0) ? -1 : 1;
			int rungIncr = incr * pa->pitchLadderDirectionHeight;

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
				if(pa->hdr.flags & RF_CAPTION)
				{
					char sz[6];
					RECT rc;

					snprintf(sz, sizeof(sz), "%d", abs(currentUnit));

					int captionIncr = incr * (pa->pitchLadderDirectionHeight + pa->font->charheight);

					POINT pts[2]	= { { ctr.x - pa->centreClearance, currentPos + captionIncr }, { ctr.x + pa->centreClearance, currentPos  + captionIncr } };
					rotatePts(pts, 2, &ctr, rollValue);

					rc.width 	= pa->font->charwidth * 2;
					rc.height	= pa->font->charheight;
					rc.left		= pts[0].x;
					rc.top		= pts[0].y;

					drawText(&rc, pa->font, Fill, sz);

					rc.left		= pts[1].x;
					rc.top		= pts[1].y;
					drawText(&rc, pa->font, Fill, sz);
				}

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

		if(pa->hdr.flags & RF_OUTLINE)
		{
			offsetPts((PPOINT)lines, lcnt << 1, 0, -1);
			drawLines(lines, lcnt, Inverse, &pa->hdr.rect);
		}

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

void renderVertPointer(PRECT rect, RenderFlagsType rf)
{
	DU halfWidth 	= rect->width >> 1;

	POINT pt[4];
	if(rf & RF_ALIGN_BOTTOM)
	{
		pt[0].x		= rect->left;
		pt[0].y		= rect->top + rect->height;
		pt[1].x		= rect->left + halfWidth;
		pt[1].y		= rect->top;
		pt[2].x		= rect->left + (halfWidth * 2);
		pt[2].y		= rect->top + rect->height;
		pt[3].x		= rect->left;
		pt[3].y		= rect->top + rect->height;
	}
	else
	{
		pt[0].x		= rect->left;
		pt[0].y		= rect->top;
		pt[1].x		= rect->left + halfWidth;
		pt[1].y		= rect->top + rect->height;
		pt[2].x		= rect->left + (halfWidth * 2);
		pt[2].y		= rect->top;
		pt[3].x		= rect->left;
		pt[3].y		= rect->top;
	}

	if(rf & RF_OUTLINE)
	{
		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), Inverse, NULL);

		if(rf & RF_ALIGN_BOTTOM)
		{
			pt[0].x++;
			pt[0].y--;
			pt[1].y++;
			pt[2].x--;
			pt[2].y--;
			pt[3].x++;
			pt[3].y--;
		}
		else
		{
			pt[0].x++;
			pt[0].y++;
			pt[1].y--;
			pt[2].x--;
			pt[2].y++;
			pt[3].x++;
			pt[3].y++;
		}

		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), None, NULL);
	}
	else
	{
		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), None, NULL);
	}
}

void renderHorzPointer(PRECT rc, RenderFlagsType rf)
{
	POINT pt[6];
	DU hh 	= rc->height >> 1;

	if(rf & RF_ALIGN_RIGHT)
	{
		pt[0].x = rc->left;
		pt[0].y	= rc->top;
		pt[1].x	= rc->left + rc->width - hh;
		pt[1].y = rc->top;
		pt[2].x = rc->left + rc->width;
		pt[2].y = pt[1].y + hh;
		pt[3].x = pt[1].x;
		pt[3].y = rc->top + hh * 2;
		pt[4].x = rc->left;
		pt[4].y = pt[3].y;
		pt[5].x = rc->left;
		pt[5].y = rc->top;
	}
	else
	{
		pt[0].x = rc->left + hh;
		pt[0].y	= rc->top;
		pt[1].x	= rc->left + rc->width;
		pt[1].y = rc->top;
		pt[2].x = pt[1].x;
		pt[2].y = rc->top + hh * 2;
		pt[3].x = rc->left + hh;
		pt[3].y = rc->top + hh * 2;
		pt[4].x = rc->left;
		pt[4].y = rc->top + hh;
		pt[5].x = rc->left + hh;
		pt[5].y = rc->top;
	}

	if(rf & RF_OUTLINE)
	{
		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), Inverse, NULL);

		if(rf & RF_ALIGN_RIGHT)
		{
			pt[0].x++;
			pt[0].y++;
			pt[1].y++;
			pt[2].x--;
			pt[3].y--;
			pt[4].x++;
			pt[4].y--;
			pt[5].x++;
			pt[5].y++;
		}
		else
		{
			pt[0].y++;
			pt[1].x--;
			pt[1].y++;
			pt[2].x--;
			pt[2].y--;
			pt[3].y--;
			pt[4].x++;
			pt[5].y++;
			pt[5].x++;
		}

		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), None, NULL);
	}
	else
	{
		drawPolyLine(pt, sizeof(pt) / sizeof(POINT), None, NULL);
	}
}

void renderHeadingTape(PRENDERER r)
{
	char sz[6];
	LINE lines[LINE_RENDER_BATCH];

	RECT rc;

	PSCALE pt 		= (PSCALE)r;
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
					snprintf(sz, sizeof(sz), "N");
					break;
				case 90:
					snprintf(sz, sizeof(sz), "E");
					break;
				case 180:
					snprintf(sz, sizeof(sz), "S");
					break;
				case -90:
				case 270:
					snprintf(sz, sizeof(sz), "W");
					break;
				default:
					snprintf(sz, sizeof(sz), "%d", currentUnit);
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

		drawLines(lines, lcnt, None, NULL);
		if(pt->hdr.flags & RF_OUTLINE)
		{
			offsetPts((PPOINT)lines, lcnt << 1, -1, 0);
			drawLines(lines, lcnt, Inverse, NULL);
		}

		divs -= lcnt;
	}

	const DU cx	= 7;
	rc.left 	= midPoint - (cx >> 1);
	rc.top		= (pt->hdr.flags & RF_ALIGN_BOTTOM) ? pt->hdr.rect.top + pt->hdr.rect.height + 2 : pt->hdr.rect.top - cx - 2;
	rc.width	= cx;
	rc.height	= cx;

	renderVertPointer(&rc, pt->hdr.flags);

	rc.left		= midPoint - (((pt->font->charwidth) * 3) >> 1);
	rc.top		+= (pt->hdr.flags & RF_ALIGN_BOTTOM) ? cx : -cx;
	rc.width	= (pt->font->charwidth * 3) + 3;
	rc.height	= pt->font->charheight + 5;

	if(pt->hdr.flags & RF_OUTLINE)
	{
		drawRect(&rc, Inverse | Outline);
		inflateRect(&rc, -1, -1);
		drawRect(&rc, Outline);
	}
	else
	{
		drawRect(&rc, Outline);
	}

	snprintf(sz, sizeof(sz), "%d", (int)truncf(value));

	rc.left = midPoint - (((strlen(sz) * pt->font->charwidth) >> 1)-1);
	rc.top += 2;

	drawText(&rc, pt->font, pt->hdr.flags & RF_OUTLINE, sz);

	selectForeColour(ofc);
}

void renderTape(PRENDERER r)
{
	PSCALE pt = (PSCALE)r;

	char sz[6];
	LINE lines[LINE_RENDER_BATCH];
	RECT rc;

	COLOUR ofc		= selectForeColour(pt->hdr.colour);
	DrawFlags df	= pt->hdr.flags & (RF_ALIGN_RIGHT | RF_OUTLINE);

	float value		= DEREFERENCE_OFFSET_FLOAT(pt->valueOffset);
	if(value < pt->minValue) value = pt->minValue; else if(value > pt->maxValue) value = pt->maxValue;

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

				snprintf(sz, sizeof(sz), "%d", currentUnit);

				drawText(&rc, pt->font, df | df2, sz);
			}

			currentUnit		-= pt->unitsPerDivision;
			startPoint		+= pt->pixelsPerDivision;
		}

		drawLines(lines, lcnt, None, NULL);
		if(pt->hdr.flags & RF_OUTLINE)
		{
			offsetPts((PPOINT)lines, lcnt << 1, 0, -1);
			drawLines(lines, lcnt, Inverse, NULL);
		}

		divs -= lcnt;
	}

	DU arrowHeight			= pt->font->charheight + 6;
	DU arrowWidth			= (pt->font->charwidth * 4) + (pt->font->charheight >> 1) + 4;

	rc.left					= (pt->hdr.flags & RF_ALIGN_RIGHT) ? (pt->hdr.rect.left + pt->hdr.rect.width) - arrowWidth : pt->hdr.rect.left;
	rc.top					= midPoint - (arrowHeight >> 1);
	rc.width				= arrowWidth;
	rc.height				= arrowHeight;

	renderHorzPointer(&rc, pt->hdr.flags);

	rc.top += 3;
	rc.left += (pt->hdr.flags & RF_ALIGN_RIGHT) ? 1 : arrowHeight >> 1;
	rc.width -= arrowHeight >> 1;

	snprintf(sz, sizeof(sz), "%d", (int)truncf(value));

	drawText(&rc, pt->font, df, sz);

	selectForeColour(ofc);
}

void renderVerticalSlider(PRENDERER r)
{
	PSCALE ps = (PSCALE)r;
	DU divs				= ps->maxValue - ps->minValue;
	DU pixelsPerDiv 	= ps->hdr.rect.height / divs;
	DU minDivLeft;
	DU majDivLeft;
	DrawFlags df		= ps->hdr.flags & RF_OUTLINE;

	COLOUR ofc			= selectForeColour(ps->hdr.colour);

	LINE lines[LINE_RENDER_BATCH];

	char sz[6];

	RECT rc;

	if(ps->hdr.flags & RF_ALIGN_RIGHT)
	{
		minDivLeft		= majDivLeft		= ps->hdr.rect.left;
	}
	else
	{
		minDivLeft		= (ps->hdr.rect.left + ps->hdr.rect.width) - ps->minorDivisionWidth;
		majDivLeft		= (ps->hdr.rect.left + ps->hdr.rect.width) - ps->majorDivisionWidth;
	}

	int curLine			= 0;
	int curValue		= ps->maxValue;
	int majorUnit		= ps->unitsPerDivision * ps->majorDivisionIntervals;
	DU curY				= ps->hdr.rect.top;

	for(int i=0; i<=divs; i++)
	{
		lines[curLine].p1.y	= lines[curLine].p2.y		= curY;

		if(curValue % majorUnit == 0)
		{
			lines[curLine].p1.x	= majDivLeft;
			lines[curLine].p2.x	= majDivLeft + ps->majorDivisionWidth;

			rc.left		= (ps->hdr.flags & RF_ALIGN_RIGHT) ? ps->hdr.rect.left + ps->majorDivisionWidth + 2 : ps->hdr.rect.left;
			rc.top		= curY - (ps->font->charheight >> 1);
			rc.width	= ps->hdr.rect.width - (ps->minorDivisionWidth + 2);
			rc.height	= ps->font->charheight;

			snprintf(sz, sizeof(sz), "%d", curValue);

			drawText(&rc, ps->font, df, sz);
		}
		else
		{
			lines[curLine].p1.x	= minDivLeft;
			lines[curLine].p2.x	= minDivLeft + ps->minorDivisionWidth;
		}

		curY		+= pixelsPerDiv;
		curValue	-= ps->unitsPerDivision;

		if(++curLine == LINE_RENDER_BATCH || i == divs)
		{
			drawLines(lines, curLine, None, NULL);
			if(ps->hdr.flags & RF_OUTLINE)
			{
				offsetPts((PPOINT)lines, curLine << 1, 0, -1);
				drawLines(lines, curLine, Inverse, NULL);
			}

			curLine = 0;
		}
	}

	float value		= DEREFERENCE_OFFSET_FLOAT(ps->valueOffset);
	if(value < ps->minValue) value = ps->minValue; else if(value > ps->maxValue) value = ps->maxValue;

	DU offset	= truncf(((ps->maxValue - value) / (float)ps->unitsPerDivision) * (float)pixelsPerDiv);

	rc.left		= (ps->hdr.flags & RF_ALIGN_RIGHT) ? ps->hdr.rect.left - 5 : ps->hdr.rect.left + ps->hdr.rect.width;
	rc.top		= (ps->hdr.rect.top + offset) - 2;
	rc.width 	= rc.height		= 5;

	renderHorzPointer(&rc, ps->hdr.flags);

	selectForeColour(ofc);
}

extern FONT systemFont;
extern FONT tinyFont;
extern FONT tinyFont2;
extern FONT minimFont;

void INTERRUPT PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	RECT rc = { 100, 40, 200, 20 };
	COLOUR cols[] = {
			RED,
			ORANGE,
			GREEN,
			LTGREEN,
			BLUE,
			AQUAMARINE,
			YELLOW,
			MAGENTA,
			CYAN,
			PURPLE,
			WHITE,
			BLACK,
			GRAY,
			DKGRAY
	};

	PFONT font = &systemFont;
	for(int i=0; i<14; i++)
	{
		for(int j=0; j<3; j++)
		{
			DrawFlags df;
			switch(j)
			{
			case 0:
				df 			= Outline;
				break;
			case 1:
				df			= Fill;
				break;
			case 2:
				df			= Inverse | Fill;
				break;
			}

			rc.left			= 30 + (60 * j);

			selectForeColour(cols[i]);
			drawText(&rc, font, df, "WEKA OSD! 0123456789");
		}

		rc.top += font->charheight + 1;
	}

	RECT rc2 = { 05, 40, 100, 10 };
	selectForeColour(WHITE);
	drawText(&rc2, &tinyFont2, Outline, "-./0123456789");

	/*
	for(PRENDERER* p = renderers; *p != NULL; p++)
	{
		PRENDERER r = *p;

		RenderFlagsType rf = r->flags;
		if((rf & RF_HIDDEN) || ((rf & RF_BLINK) &&  blinkOn()))
			continue;

		r->renderProc(r);
	}
	*/

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
