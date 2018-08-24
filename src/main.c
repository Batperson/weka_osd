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

  PA0			HSYNC input
  PA2			VSYNC input
  PA3			FIELD input
  PB6-7			I2C SCL and SDA output
  PC6			Pixel clock output for debugging
  PF0-7			Pixel bus output
  PF9-10		LEDs, active low

  TIM2			Video line counter
  TIM8			Pixel clock for DMA
*/


/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
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

  //DBGMCU_APB1PeriphConfig(DBGMCU_TIM8_STOP, ENABLE);
  //DBGMCU_APB2PeriphConfig(DBGMCU_TIM2_STOP, ENABLE);
  //DBGMCU_APB2PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);

  /* TODO - Add your application code here */
  printf("Initializing hardware...\r\n");

  initLeds();
  initI2C1();
  initVideoChips();
  initSystem();

  setFastBlankContrastReductionMode(FBContrastReductionDisabled);
  setFastBlankMode(FBModeDynamic);

  initVideo();

  while (1)
  {

  }
}
