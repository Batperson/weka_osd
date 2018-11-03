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

	RECT rc = { 1, 10, 298, 100 };
	drawRect(&rc, RED, GRAY);

	RECT rc2 = { 60, 120, 120, 20 };
	drawTestPattern(&rc2);

	POINT pts[] = {
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 },
			{ 100, 100 }, { 100, 110 }
	};
	POINT ctr = { 100, 120 };

	int rays = (sizeof(pts) / sizeof(POINT)) / 2;
	float angle = 360.0 / (float)rays;
	for(int i=0; i<rays ; i += 2)
		rotatePts(pts+i, 2, &ctr, i * angle);

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;
}
