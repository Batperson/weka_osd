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
#include "model.h"

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

void INTERRUPT IN_CCM PendSV_Handler()
{
	// Reset cycle counter
	DWT->CYCCNT = 0;

	clearRenderBuf();

	RECT rc = { 20, 10, 120, 20 };
	drawTestPattern(&rc);

	renderArtificialHorizon();

	// Output cycle count for profiling
	ITM_Port32(1)	= DWT->CYCCNT;

	// For the time being, update model values from here
	demoModelUpdate();
}
