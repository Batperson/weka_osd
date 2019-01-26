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
#include "float.h"
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
extern RENDERPROC renderLabel;
extern RENDERPROC renderDynLabel;
extern RENDERPROC renderVerticalSlider;

extern FONT systemFont;
extern FONT tinyFont;

PRENDERER allocRenderer(size_t size)
{
	PRENDERER renderer	= (PRENDERER)malloc(size);
	mset(renderer, 0, size);

	renderer->size		= size;
	renderer->flags		= 0;
	renderer->colour	= RGB(3,3,2);	// Default colour

	return renderer;
}

PSEGMENT allocSegments(int cnt)
{
	size_t size = sizeof(SEGMENT) * cnt;
	PSEGMENT segs = (PSEGMENT)malloc(size);
	mset(segs, 0, size);

	return segs;
}

PSCALE allocTape()
{
	PSCALE tape						= (PSCALE)allocRenderer(sizeof(SCALE));
	tape->hdr.renderProc			= (RENDERPROC)&renderTape;
	tape->hdr.flags					= RF_OUTLINE;
	tape->valueOffset				= offsetof(MODEL, vel.horizontal);
	tape->minValue					= -FLT_MAX;
	tape->maxValue					= FLT_MAX;
	tape->unitsPerDivision			= 1;
	tape->pixelsPerDivision			= 20;
	tape->majorDivisionIntervals	= 5;
	tape->majorDivisionWidth		= 4;
	tape->minorDivisionWidth		= 2;
	tape->font						= &systemFont;

	return tape;
}

PSCALE allocHeadingTape()
{
	PSCALE tape						= allocTape();
	tape->hdr.renderProc			= (RENDERPROC)&renderHeadingTape;
	tape->hdr.flags				 	= RF_OUTLINE;
	tape->unitsPerDivision			= 2;
	tape->valueOffset				= offsetof(MODEL, att.heading);

	return tape;
}

PSCALE allocSlider()
{
	PSCALE slider					= (PSCALE)allocRenderer(sizeof(SCALE));
	slider->hdr.renderProc			= (RENDERPROC)&renderVerticalSlider;
	slider->hdr.flags				= RF_OUTLINE;
	slider->valueOffset				= offsetof(MODEL, vel.vertical);
	slider->minValue				= -5;
	slider->maxValue				= 5;
	slider->unitsPerDivision		= 1;
	slider->pixelsPerDivision		= 6;
	slider->majorDivisionIntervals	= 2;
	slider->majorDivisionWidth		= 4;
	slider->minorDivisionWidth		= 2;
	slider->font					= &tinyFont;

	return slider;
}

PARROW allocArrow()
{
	PARROW arrow					= (PARROW)allocRenderer(sizeof(ARROW));
	arrow->hdr.flags				= RF_OUTLINE;
	arrow->hdr.renderProc			= (RENDERPROC)&renderArrow;
	arrow->valueOffset				= offsetof(MODEL, att.homeVector);

	return arrow;
}

PINDICATOR allocBarMeter()
{
	PINDICATOR indicator			= (PINDICATOR)allocRenderer(sizeof(INDICATOR));
	indicator->hdr.renderProc		= (RENDERPROC)&renderBarMeter;
	indicator->range.cnt			= 0;
	indicator->range.segments		= NULL;
	indicator->range.min			= 0;
	indicator->range.max			= 0;

	return indicator;
}

PINDICATOR allocBatteryMeter()
{
	PINDICATOR indicator			= allocBarMeter();
	indicator->hdr.renderProc		= (RENDERPROC)&renderBatteryMeter;
	indicator->valueOffset			= offsetof(MODEL, elec.voltage);

	return indicator;
}

PAHI allocArtificialHorizon()
{
	PAHI ahi						= (PAHI)allocRenderer(sizeof(AHI));
	ahi->hdr.flags					= RF_OUTLINE | RF_CAPTION;
	ahi->hdr.renderProc				= (RENDERPROC)&renderArtificialHorizon;
	ahi->pitchValueOffset			= offsetof(MODEL, att.pitch);
	ahi->rollValueOffset			= offsetof(MODEL, att.roll);
	ahi->unitsPerDivision			= 10;
	ahi->pixelsPerDivision			= 60;
	ahi->pitchLadderWidth			= 30;
	ahi->pitchLadderDirectionHeight	= 5;
	ahi->horizonLineWidth			= 80;
	ahi->centreClearance			= 20;
	ahi->font						= &tinyFont;

	return ahi;
}

PLABEL allocLabel()
{
	PLABEL label					= (PLABEL)allocRenderer(sizeof(LABEL));
	label->hdr.renderProc			= (RENDERPROC)&renderLabel;
	label->font						= &systemFont;
	label->text						= NULL;

	return label;
}

PDYNLABEL allocDynLabel()
{
	PDYNLABEL label					= (PDYNLABEL)allocRenderer(sizeof(DYNLABEL));
	label->hdr.renderProc			= (RENDERPROC)&renderDynLabel;
	label->font						= &systemFont;
	label->format					= "%.1f";
	label->valueOffset				= 0;

	return label;
}

void initRenderers()
{
	int rendererLength = 13;
	renderers 					= (PRENDERER*)malloc(sizeof(void*) * (rendererLength + 1));

	PSCALE altitude				= allocTape();
	initRect(&altitude->hdr.rect, 338, 0, 20, 288);
	altitude->hdr.flags			= RF_ALIGN_RIGHT | RF_OUTLINE;
	altitude->valueOffset		= offsetof(MODEL, loc.altitude);

	PSCALE airspeed				= allocTape();
	initRect(&airspeed->hdr.rect, 1, 0, 20, 288);
	airspeed->hdr.flags			= RF_ALIGN_LEFT | RF_OUTLINE;
	airspeed->valueOffset		= offsetof(MODEL, vel.horizontal);

	PSCALE heading				= allocHeadingTape();
	initRect(&heading->hdr.rect, 30, 238, 300, 20);
	heading->hdr.flags			= RF_ALIGN_BOTTOM | RF_OUTLINE;

	PARROW arrow				= allocArrow();
	initRect(&arrow->hdr.rect, 300, 50, 10, 20);

	PAHI ahi					= allocArtificialHorizon();
	initRect(&ahi->hdr.rect, 0, 0, 360, 288);

	PSEGMENT segs				= allocSegments(4);
	segs[0].to					= 9.6f;
	segs[1].to					= 10.0f;
	segs[2].to					= 10.8f;
	segs[3].to					= 12.6f;
	segs[0].colour				= RED;
	segs[1].colour				= ORANGE;
	segs[2].colour				= YELLOW;
	segs[3].colour				= GREEN;

	PINDICATOR battery			= allocBatteryMeter();
	initRect(&battery->hdr.rect, 30, 20, 25, 12);
	battery->hdr.flags			= RF_ALIGN_RIGHT | RF_OUTLINE;
	battery->range.min			= 9.0f;
	battery->range.max			= 12.6f;
	battery->range.cnt			= 4;
	battery->range.segments		= segs;

	segs						= allocSegments(3);
	segs[0].to					= 40.0;
	segs[1].to					= 50.0f;
	segs[2].to					= 60.0f;
	segs[0].colour				= GREEN;
	segs[1].colour				= YELLOW;
	segs[2].colour				= RED;

	PINDICATOR current			= allocBarMeter();
	initRect(&current->hdr.rect, 30, 34, 25, 12);
	current->hdr.flags			= RF_ALIGN_LEFT | RF_OUTLINE;
	current->valueOffset		= offsetof(MODEL, elec.current);
	current->range.min			= 0.0f;
	current->range.max			= 60.0f;
	current->range.cnt			= 3;
	current->range.segments		= segs;

	PLABEL status				= allocLabel();
	initRect(&status->hdr.rect, 300, 265, 60, 20);
	status->hdr.colour			= RGB(3,3,0);
	status->hdr.flags			= RF_INVERSE | RF_BLINK;
	status->text				= "ARMED";

	PSCALE vario				= allocSlider();
	initRect(&vario->hdr.rect, 300, 130, 20, 90);
	vario->hdr.flags			= RF_ALIGN_RIGHT | RF_OUTLINE;

	PDYNLABEL varioVal			= allocDynLabel();
	initRect(&varioVal->hdr.rect, 282, 221, 30, 10);
	varioVal->hdr.flags			= RF_OUTLINE | RF_ALIGN_RIGHT;
	varioVal->format			= "%.1f";
	varioVal->valueOffset		= offsetof(MODEL, vel.vertical);

	PDYNLABEL batVal			= allocDynLabel();
	initRect(&batVal->hdr.rect, 56, 22, 40, 12);
	batVal->hdr.flags			= RF_OUTLINE;
	batVal->format				= "%.1fV";
	batVal->valueOffset			= offsetof(MODEL, elec.voltage);

	PDYNLABEL ampVal			= allocDynLabel();
	initRect(&ampVal->hdr.rect, 56, 36, 40, 12);
	ampVal->hdr.flags			= RF_OUTLINE;
	ampVal->format				= "%.1fA";
	ampVal->valueOffset			= offsetof(MODEL, elec.current);

	int i = 0;
	renderers[i++]				= (PRENDERER)ahi;
	renderers[i++]				= (PRENDERER)altitude;
	renderers[i++]				= (PRENDERER)airspeed;
	renderers[i++]				= (PRENDERER)heading;
	renderers[i++]				= (PRENDERER)arrow;
	renderers[i++]				= (PRENDERER)battery;
	renderers[i++]				= (PRENDERER)current;
	renderers[i++]				= (PRENDERER)status;
	renderers[i++]				= (PRENDERER)vario;
	renderers[i++]				= (PRENDERER)varioVal;
	renderers[i++]				= (PRENDERER)batVal;
	renderers[i++]				= (PRENDERER)ampVal;
	renderers[i++]				= NULL;
}
