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

const u32 blinkInterval = 300;

void sleep(u32 millisecs)
{
	u32 cyccnt = DWT->CYCCNT + (SystemCoreClock / (1000000 / millisecs));
	while(DWT->CYCCNT < cyccnt)
		;
}

void IN_CCM SysTick_Handler()
{
	if(++sysTicks >= sysTickBlinkThreshold)
	{
		sysTickBlinkThreshold = sysTicks + blinkInterval;
		blink = !blink;

		toggleLed2();
	}
}

void initSystem()
{
	sysTicks = 0;
	sysTickBlinkThreshold = blinkInterval;

	RCC_ClocksTypeDef clk;
	RCC_GetClocksFreq(&clk);

	SysTick_Config(clk.HCLK_Frequency  / 1000);
}

