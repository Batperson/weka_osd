/*
 * model.c
 *
 *  Created on: 5/11/2018
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "model.h"
#include "math.h"
#include "string.h"
#include "memory.h"

MODEL model;

void initModel()
{
	mset(&model, 0, sizeof(MODEL));
}

void demoModelUpdate()
{
	static float inc = 0;

	inc += 0.05;
	float val1 = inc;
	float val2 = inc / 20;

	model.att.heading		= ((cosf(val1) * -60)/4) + 90;
	model.att.homeVector	+= 2;
	model.att.roll 			= (sinf(val1) * 60);
	model.loc.altitude 		= (sinf(val2) * 40) + 50;
	model.vel.vertical		= (cosf(val2) * 5);
	model.vel.horizontal	= (cosf(val2) * 10) + 20;
	model.elec.voltage		= (sinf(val2) * 1.8) + 10.8;
	model.elec.current		= (sinf(val1) * 20) + 40;
}
