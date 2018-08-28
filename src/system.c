/*
 * system.c
 *
 *  Created on: 21/08/2018
 */
#include "stm32f4xx.h"
#include "misc.h"
#include "bitband.h"
#include "control.h"
#include "system.h"


volatile u32 sysTicks;
volatile u32 sysTickBlinkThreshold;
volatile u8 blink;

const u32 blinkInterval = 500;

void INTERRUPT IN_CCM SysTick_Handler()
{
	if(++sysTicks >= sysTickBlinkThreshold)
	{
		sysTickBlinkThreshold = sysTicks + blinkInterval;
		blink = !blink;

		toggleLed2();
	}
}

ALWAYS_INLINE u8 blinkOn() { return blink; }
ALWAYS_INLINE u32 millis() { return sysTicks; }

void initSystem()
{
	sysTicks = 0;
	sysTickBlinkThreshold = blinkInterval;

	RCC_ClocksTypeDef clk;
	RCC_GetClocksFreq(&clk);

	SysTick_Config(clk.HCLK_Frequency  / 1000);
}

