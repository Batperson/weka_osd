#include "stdio.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "bitband.h"
#include "i2c.h"
#include "control.h"
#include "system.h"
#include "graphics.h"
#include "model.h"
#include "video.h"
#include "configuration.h"

/*
  Pin and peripheral Usage:

  PA0				HSYNC input (pixel start)
  PA1				VSYNC input
  PA2				FIELD input
  PA3				INT input
  PA5				HSYNC input (line count)

  PB8-9				I2C SCL and SDA output

  PC13				User button

  PF0-7				Pixel bus output
  PB0, PB7, PB14	LEDs

  TIM2				Video line counter
  TIM8				Pixel clock for DMA
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

  printf("WekaOSD 0.01\n");

  SystemCoreClockUpdate();
  initDebug();

  printf("Initializing hardware...\n");

  initSystem();
  initLeds();
  initModel();
  initUserButtons();
  initI2C1();
  initVideoChips();
  initRenderers();
  initVideo();

  //setLowPowerMode(LowPowerDAC1);

  while (1)
  {
  }
}


u8 chroma = 0;
void INTERRUPT EXTI15_10_IRQHandler()
{
	EXTI_ClearITPendingBit(EXTI_Line13);


}
