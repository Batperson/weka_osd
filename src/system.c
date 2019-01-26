/*
 * system.c
 * system and OS-like utility functions
 *  Created on: 21/08/2018
 */
#include "stm32f4xx.h"
#include "misc.h"
#include "bitband.h"
#include "control.h"
#include "system.h"


volatile u32 sysTicks;
volatile u32 sysTickBlinkThreshold;
volatile u32 sysTickSleepThreshold;
volatile u8 blink;

const u32 blinkInterval = 300;

void sleep(u32 millisecs)
{
	sysTickSleepThreshold = sysTicks + millisecs;
	while(sysTicks < sysTickSleepThreshold)
		;
}

void initSystem()
{
	sysTicks = 0;
	sysTickBlinkThreshold = blinkInterval;

	RCC_ClocksTypeDef clk;
	RCC_GetClocksFreq(&clk);

	/* Set systick to tick every 1 millisecs */
	SysTick_Config(clk.HCLK_Frequency  / 1000);
}

void INTERRUPT SysTick_Handler()
{
	if(++sysTicks >= sysTickBlinkThreshold)
	{
		sysTickBlinkThreshold = sysTicks + blinkInterval;
		blink = !blink;

		toggleLed2();
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void INTERRUPT HardFault_Handler()
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  for(int i=0; i<4000000; i++)
		  ;

	  toggleLed3();
  }
}

