/*
 * configuration.c
 * Run-time configuration and deserialization
 *
 *  Created on: 24/12/2018
 */

#include "stm32f4xx.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "misc.h"
#include "system.h"
#include "memory.h"
#include "graphics.h"
#include "render.h"
#include "model.h"
#include "configuration.h"

extern RENDERPROC renderTape;
extern RENDERPROC renderHeadingTape;
extern RENDERPROC renderArrow;
extern RENDERPROC renderArtificialHorizon;
extern RENDERPROC renderBatteryMeter;
extern RENDERPROC renderBarMeter;

extern FONT systemFont;
extern FONT tinyFont;

PRENDERER allocRenderer(size_t size)
{
	PRENDERER renderer	= (PRENDERER)malloc(size);
	renderer->size		= size;
	renderer->flags		= 0;
	renderer->colour	= RGB(2,3,2);	// Default colour

	initRect(&renderer->rect, 0, 0, 0, 0);

	return renderer;
}

PTAPE allocTape()
{
	PTAPE tape						= (PTAPE)allocRenderer(sizeof(TAPE));
	tape->hdr.renderProc			= renderTape;
	tape->valueOffset				= offsetof(MODEL, vel.horizontal);
	tape->unitsPerDivision			= 1;
	tape->pixelsPerDivision			= 20;
	tape->majorDivisionIntervals	= 5;
	tape->majorDivisionWidth		= 4;
	tape->minorDivisionWidth		= 2;
	tape->font						= &systemFont;

	return tape;
}

PTAPE allocHeadingTape()
{
	PTAPE tape						= allocTape();
	tape->hdr.renderProc			= renderHeadingTape;
	tape->unitsPerDivision			= 2;
	tape->valueOffset				= offsetof(MODEL, att.heading);

	return tape;
}

PARROW allocArrow()
{
	PARROW arrow					= (PARROW)allocRenderer(sizeof(ARROW));
	arrow->hdr.renderProc			= renderArrow;
	arrow->valueOffset				= offsetof(MODEL, att.homeVector);

	return arrow;
}

PINDICATOR allocBarMeter()
{
	PINDICATOR indicator			= (PINDICATOR)allocRenderer(sizeof(INDICATOR));
	indicator->hdr.renderProc		= renderBarMeter;
	indicator->range.cnt			= 0;
	indicator->range.segments		= NULL;
	indicator->range.min			= 0;
	indicator->range.max			= 0;
	indicator->font					= &systemFont;

	return indicator;
}

PINDICATOR allocBatteryMeter()
{
	PINDICATOR indicator			= allocBarMeter();
	indicator->hdr.renderProc		= renderBatteryMeter;
	indicator->valueOffset			= offsetof(MODEL, elec.voltage);

	return indicator;
}

PAHI allocArtificialHorizon()
{
	PAHI ahi						= (PAHI)allocRenderer(sizeof(AHI));
	ahi->hdr.renderProc				= renderArtificialHorizon;
	ahi->pitchValueOffset			= offsetof(MODEL, att.pitch);
	ahi->rollValueOffset			= offsetof(MODEL, att.roll);
	ahi->unitsPerDivision			= 10;
	ahi->pixelsPerDivision			= 60;
	ahi->pitchLadderWidth			= 30;
	ahi->pitchLadderDirectionHeight	= 5;
	ahi->horizonLineWidth			= 80;
	ahi->centreClearance			= 20;

	return ahi;
}

void initRenderers()
{
	int rendererLength = 8;
	renderers 					= (PRENDERER*)malloc(sizeof(void*) * rendererLength);

	PTAPE altitude				= allocTape();
	initRect(&altitude->hdr.rect, 338, 0, 20, 288);
	altitude->hdr.flags			= RF_ALIGN_RIGHT;
	altitude->valueOffset		= offsetof(MODEL, loc.altitude);

	PTAPE airspeed				= allocTape();
	initRect(&airspeed->hdr.rect, 1, 0, 20, 288);
	altitude->valueOffset		= offsetof(MODEL, vel.horizontal);

	PTAPE heading				= allocHeadingTape();
	initRect(&heading->hdr.rect, 30, 238, 300, 20);
	heading->hdr.flags			= RF_ALIGN_BOTTOM;

	PARROW arrow				= allocArrow();
	initRect(&arrow->hdr.rect, 300, 50, 10, 20);

	PAHI ahi					= allocArtificialHorizon();
	initRect(&ahi->hdr.rect, 0, 0, 360, 288);

	PINDICATOR battery			= allocBatteryMeter();
	initRect(&battery->hdr.rect, 30, 20, 25, 12);
	battery->hdr.flags			= RF_ALIGN_LEFT | RF_CAPTION | RF_OUTLINE;
	battery->range.min			= 9.0f;
	battery->range.max			= 12.6f;
	battery->format				= "%.1fV";

	PINDICATOR current			= allocBarMeter();
	initRect(&current->hdr.rect, 30, 34, 25, 12);
	current->hdr.flags			= RF_ALIGN_LEFT | RF_CAPTION | RF_OUTLINE;
	current->range.min			= 0.0f;
	current->range.max			= 60.0f;
	current->format				= "%.1fA";

	renderers[0]				= NULL;	// TODO: status label
	renderers[1]				= &ahi->hdr;
	renderers[2]				= &altitude->hdr;
	renderers[3]				= &airspeed->hdr;
	renderers[4]				= &heading->hdr;
	renderers[5]				= &arrow->hdr;
	renderers[6]				= &battery->hdr;
	renderers[7]				= &current->hdr;
}
