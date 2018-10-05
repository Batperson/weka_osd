#include "stdio.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "bitband.h"
#include "i2c.h"
#include "control.h"
#include "system.h"
#include "video.h"

/*
  Pin and peripheral Usage:

  PA0			HSYNC input (pixel start)
  PA1			VSYNC input
  PA2			FIELD input
  PA5			HSYNC input (line count)

  PB6-7			I2C SCL and SDA output

  PC6			Pixel clock output for debugging

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

  /* NOTE - do we need this? */
  SystemCoreClockUpdate();
  initDebug();

  /* TODO - Add your application code here */
  printf("Initializing hardware...\r\n");

  /* Give the chips a chance to fully power up */
  sleep(500);

  initLeds();
  initI2C1();
  initVideoChips();
  initSystem();

  setFastBlankMode(FBModeDynamicEdgeEnhanced);
  setFastBlankContrastReductionMode(FBContrastReductionEnabled);
  setFastBlankContrastReductionLevel(FBContrastReductionLevel75);
  setFastBlankEdgeShapeLevel(FBEdgeShapeLevel4);
  setFastBlankThresholds(FBLevelThreshold3, FBContrastThreshold1);

  initVideo();

  while (1)
  {

  }
}
