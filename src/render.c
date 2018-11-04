/*
 * render.c
 *
 * Rendering of the OSD
 */

#include "stm32f4xx.h"
#include "string.h"
#include "video.h"
#include "misc.h"
#include "graphics.h"
#include "system.h"
#include "memory.h"


void INTERRUPT IN_CCM PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	RECT rc = { 1, 10, 298, 60 };
	drawRect(&rc, RED, GRAY);

	RECT rc2 = { 60, 120, 120, 20 };
	drawTestPattern(&rc2);

	POINT pts[] = {
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 },
			{ 200, 100 }, { 200, 120 }
	};
	POINT ctr = { 200, 130 };

	int rays = (sizeof(pts) / sizeof(POINT)) / 2;
	float angle = 360.0 / (float)rays;
	float initAngle = (millis() / 10);

	COLOUR clr = ((millis() / 300) % 0x40) | 0xC0;
	for(int i=0; i<rays ; i++)
	{
		rotatePts(pts+(i*2), 2, &ctr, (i * angle) + initAngle);

		LINE l = { pts[i*2], pts[i*2+1] };
		drawLine(&l, clr, NULL);
	}

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;
}
