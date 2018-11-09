/*
 * model.c
 *
 *  Created on: 5/11/2018
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "model.h"
#include "math.h"

IN_CCM MODEL model;

void demoModelUpdate()
{
	static float inc = 0;

	inc += 0.1;
	float val1 = inc;
	float val2 = inc / 40;

	model.att.roll 		= (sinf(val1) * 60);
	model.loc.altitude 	= (sinf(val2) * 40) + 50;
	model.vel.vertical	= (cosf(val2) * 10);
}
