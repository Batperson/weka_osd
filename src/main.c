#include "stdio.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "bitband.h"
#include "i2c.h"
#include "control.h"
#include "system.h"
#include "graphics.h"
#include "video.h"

/*
  Pin and peripheral Usage:

  PA0			HSYNC input (pixel start)
  PA1			VSYNC input
  PA2			FIELD input
  PA5			HSYNC input (line count)

  PB6-7			I2C SCL and SDA output

  PE3-4			User buttons

  PF0-7			Pixel bus output
  PF9-10		LEDs, active low

  TIM2			Video line counter
  TIM8			Pixel clock for DMA
*/


void initDebug()
{
	//DBGMCU_APB1PeriphConfig(DBGMCU_TIM8_STOP, ENABLE);
	//DBGMCU_APB2PeriphConfig(DBGMCU_TIM2_STOP, ENABLE);
	//DBGMCU_APB2PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);
	DWT->CTRL |= 1 ;	// Enable CPU cycle counter
}

IN_CCM char szBtn0Msg[20];
IN_CCM char szBtn1Msg[20];

int main(void)
{


  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used.
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates
  *  SCB->VTOR register.
  *  E.g.  SCB->VTOR = 0x20000000;
  */

  printf("WekaOSD 0.01\r\n");

  SystemCoreClockUpdate();
  initDebug();

  printf("Initializing hardware...\r\n");

  /* Give the chips a chance to fully power up */
  sleep(400);

  initLeds();
  initUserButtons();
  initI2C1();
  initVideoChips();
  initSystem();
  initVideo();

  sprintf(szBtn0Msg, "TEST0");
  sprintf(szBtn1Msg, "TEST1");

  while (1)
  {

  }
}

u8 fbEdge = 0;
u16 hsyncStart = 1061;

void INTERRUPT IN_CCM EXTI3_IRQHandler()
{
	EXTI_ClearITPendingBit(EXTI_Line3);

	FBModeType fm;
	if(fbEdge++ >= 8)
	{
		fm = FBModeDynamicEdgeEnhanced;
		fbEdge = 0;
	}
	else
	{
		fm = FBModeDynamicEdgeEnhanced;
	}

	setFastBlankMode(fm);
	setFastBlankEdgeShapeLevel(fbEdge);

	u8 v = I2C_ReadByte(I2C1, ADDR_DECODER, REG_DEC_FB_CONTROL3);
	sprintf(szBtn0Msg, "DNR: %d", v);
}

void INTERRUPT IN_CCM EXTI4_IRQHandler()
{
	EXTI_ClearITPendingBit(EXTI_Line4);

	if(TIM8->ARR++ >= 40)
		TIM8->ARR = 14;

	if((hsyncStart -= 14) <= 861)
		hsyncStart = 1061;

	setHSyncTiming(hsyncStart, 860);

	sprintf(szBtn1Msg, "TIM8: %d", (int)TIM8->ARR);
}
