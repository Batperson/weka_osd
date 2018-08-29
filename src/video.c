/*
 * video.c
 *
 */

#include "stm32f4xx.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "bitband.h"
#include "control.h"
#include "video.h"
#include "graphics.h"
#include "rasterize.h"

static volatile PIXEL lineBuf0[LINE_BUFFER_SIZE];
static volatile PIXEL lineBuf1[LINE_BUFFER_SIZE];
volatile PPIXEL renderBuf;

// This is working code for now, later will probably incorporate into PAL/NTSC-specific section.
static const u16 pixelOutputNanoseconds			= 52000;
static const u16 pixelsPerLine					= 768;			// PAL = 768*576, NTSC = 640*480
static const u16 activeVideoLineStart			= 23;			// NTSC starts on line 16?

u32 calcNanosecsFromAPB1TimerTicks(u16 ticks)
{
	return ((ticks + 1) * (100000000000 /  (SystemCoreClock / 2))) / 100;
}

u32 calcNanosecsFromAPB2TimerTicks(u16 ticks)
{
	return ((ticks + 1) * (100000000000 /  SystemCoreClock)) / 100;
}

u32 calcAPB1TimerPeriod(u32 timeNS)
{
	return (((SystemCoreClock / 2) / (1000000000 / timeNS)) - 1);
}

u32 calcAPB2TimerPeriod(u32 timeNS)
{
	return ((SystemCoreClock / (1000000000 / timeNS)) - 1);
}

u16 calcAPB2TimerPeriodFromHz(u32 frequency)
{
	return (SystemCoreClock / frequency) - 1;
}

u16 calcAPB1TimerPeriodFromHz(u32 frequency)
{
	return (((SystemCoreClock / 2) / frequency) - 1);
}

void initRCC()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
}

void initSyncPort()
{
	EXTI_InitTypeDef exti;
	GPIO_InitTypeDef gpio;
	NVIC_InitTypeDef nvic;
	GPIO_StructInit(&gpio);

	/* Configure PA0 for Alternate Function 1 (TIM8 ETR), datasheet page 62 */
	gpio.GPIO_Pin 		= GPIO_Pin_0;
	gpio.GPIO_Mode 		= GPIO_Mode_AF;
	gpio.GPIO_Speed 	= GPIO_Speed_50MHz;
	gpio.GPIO_OType 	= GPIO_OType_PP;
	gpio.GPIO_PuPd 		= GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &gpio);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM8);

	/* Configure PA2 and PA3 for VSYNC and FIELD */
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin 		= GPIO_Pin_2 | GPIO_Pin_3;
	gpio.GPIO_Mode 		= GPIO_Mode_IN;
	gpio.GPIO_Speed 	= GPIO_Speed_50MHz;
	gpio.GPIO_OType 	= GPIO_OType_PP;
	gpio.GPIO_PuPd 		= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);
/*
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);

	exti.EXTI_Line 		= EXTI_Line2;
	exti.EXTI_Mode 		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger 	= EXTI_Trigger_Rising;
	exti.EXTI_LineCmd 	= ENABLE;
	EXTI_Init(&exti);

	nvic.NVIC_IRQChannel 					= EXTI2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority 	= 1;
	nvic.NVIC_IRQChannelSubPriority 		= 1;
	nvic.NVIC_IRQChannelCmd 				= ENABLE;
	NVIC_Init(&nvic);
*/
}

void initPixelPort()
{
	GPIO_InitTypeDef		GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

void initPixelClock()
{
	TIM_TimeBaseInitTypeDef		timb;

	TIM_DeInit(TIM8);
	TIM_TimeBaseStructInit(&timb);

	timb.TIM_Prescaler 			= 0;
	timb.TIM_CounterMode 		= TIM_CounterMode_Up;
	timb.TIM_Period 			= calcAPB2TimerPeriod(pixelOutputNanoseconds / pixelsPerLine);
	timb.TIM_ClockDivision 		= TIM_CKD_DIV1;
	timb.TIM_RepetitionCounter 	= 0;
	TIM_TimeBaseInit(TIM8, &timb);

	TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Trigger);
	TIM_SelectInputTrigger(TIM8, TIM_TS_ETRF);
	TIM_ETRConfig(TIM8, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_Inverted, 0);
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Enable);
	TIM_DMACmd(TIM8, TIM_DMA_Update, ENABLE);

	TIM_OCInitTypeDef			ocnt;

	TIM_OCStructInit(&ocnt);

	ocnt.TIM_OCMode 			= TIM_OCMode_PWM1;
	ocnt.TIM_Pulse 				= calcAPB2TimerPeriod(pixelOutputNanoseconds / pixelsPerLine) / 2;
	ocnt.TIM_OutputState 		= TIM_OutputState_Enable;
	ocnt.TIM_OutputNState 		= TIM_OutputState_Disable;
	ocnt.TIM_OCPolarity 		= TIM_OCPolarity_Low;
	ocnt.TIM_OCIdleState 		= TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM8, &ocnt);

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM8, ENABLE);

	// Test code: toggle a GPIO pin as well, so we can observe pixel output in the logic analyzer
	// PC6 is TIM8 CH1 alternate function (p63 in datasheet)
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Pin 	= GPIO_Pin_6;
	gpio.GPIO_Mode 	= GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd 	= GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOC, &gpio);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
}

void initHSyncCount()
{
	/* Run TIM2 in internally-triggered mode.
	 * Trigger output from TIM8 (Enable event) clocks the timer.
	 * Counter is reset on VSYNC.
	 * CC1 detects when VBI is over and we need to prepare to output active video.
	 * Interrupt on CC1.
	 */
	TIM_TimeBaseInitTypeDef		timb;
	TIM_OCInitTypeDef			ocnt;
	NVIC_InitTypeDef			nvic;

	TIM_TimeBaseStructInit(&timb);
	TIM_OCStructInit(&ocnt);
	TIM_DeInit(TIM2);

	timb.TIM_Prescaler 			= 0;
	timb.TIM_CounterMode 		= TIM_CounterMode_Up;
	timb.TIM_Period 			= 65535;
	timb.TIM_ClockDivision 		= TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &timb);

	ocnt.TIM_OCMode 			= TIM_OCMode_PWM1;
	ocnt.TIM_Pulse 				= activeVideoLineStart;
	ocnt.TIM_OutputState 		= TIM_OutputState_Enable;
	ocnt.TIM_OutputNState 		= TIM_OutputState_Disable;
	ocnt.TIM_OCPolarity 		= TIM_OCPolarity_High;
	ocnt.TIM_OCIdleState 		= TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM2, &ocnt);

	// Interrupt on capture
	nvic.NVIC_IRQChannel 					= TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority 	= 0;
	nvic.NVIC_IRQChannelSubPriority 		= 0;
	nvic.NVIC_IRQChannelCmd 				= ENABLE;

	NVIC_Init(&nvic);

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_External1);
	TIM_SelectInputTrigger(TIM2, TIM_TS_ITR1);	// For TIM2, ITR1 = TIM8 TRGO, p630 reference manual
	TIM_Cmd(TIM2, ENABLE);
}

void initPixelDma()
{
	NVIC_InitTypeDef nvic;
	DMA_InitTypeDef	dmai;

	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_DeInit(DMA2_Stream1);

	// To be driven by TIM8 Update, we want DMA2 Stream1 Channel 7 (p308, reference manual)
	// Needs to be DMA2 because DMA1 does not have access to the peripherals (http://cliffle.com/article/2015/06/06/pushing-pixels/)
	DMA_StructInit(&dmai);
	dmai.DMA_PeripheralBaseAddr 	= (u32)&GPIOF->ODR;
	dmai.DMA_Memory0BaseAddr 		= (u32)lineBuf0;
	dmai.DMA_DIR 					= DMA_DIR_MemoryToPeripheral;
	dmai.DMA_BufferSize 			= pixelsPerLine + 8;
	dmai.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
	dmai.DMA_MemoryInc 				= DMA_MemoryInc_Enable;
	dmai.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
	dmai.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Word;
	dmai.DMA_Mode 					= DMA_Mode_Normal;
	dmai.DMA_Priority 				= DMA_Priority_VeryHigh;
	dmai.DMA_Channel 				= DMA_Channel_7;
	dmai.DMA_FIFOMode 				= DMA_FIFOMode_Enable;
	dmai.DMA_FIFOThreshold 			= DMA_FIFOThreshold_Full;
	dmai.DMA_MemoryBurst 			= DMA_MemoryBurst_INC4;
	dmai.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &dmai);

	nvic.NVIC_IRQChannel 						= DMA2_Stream1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority 		= 1;
	nvic.NVIC_IRQChannelSubPriority 			= 0;
	nvic.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&nvic);

	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
	DMA_Cmd(DMA2_Stream1, ENABLE);
}

void prepareNextScanLine()
{
	// Swap buffers
	renderBuf = (PPIXEL)DMA2_Stream1->M0AR;
	DMA2_Stream1->M0AR = (u32)((renderBuf == lineBuf0) ? lineBuf1 : lineBuf0);

	// Re-enable DMA for next scanline. NDTR and M0AR will automatically reload to their original values.
	DMA2_Stream1->CR |= DMA_SxCR_EN;

	// Re-enable slave mode on TIM8 so that on next TIM2 update, TIM8 starts
	TIM8->SMCR = TIM_SlaveMode_Trigger | TIM_TS_ETRF; // TIM_TS_ITR1;

	// For now, simply call this function from the interrupt. Later, will want to
	// do a context switch to it.
	rasterizeNextScanLine();
}

void __attribute__((interrupt("IRQ") /*,section(".ccmram")*/)) DMA2_Stream1_IRQHandler()
{
	ITM_Port32(1)					= 1;	// Trace
	// Clear interrupt flags
	DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1;

	// Disable slave mode, can't stop the timer unless we do this first
	TIM8->SMCR = 0;

	// Stop TIM8, TIM1 will re-enable it
	TIM8->CR1 = 0;
	TIM8->EGR = TIM_EventSource_Update;		// Trigger an Update to reset the counter

	prepareNextScanLine();

	ITM_Port32(1)					= 5;	// Trace
}

void __attribute__((interrupt("IRQ"))) TIM2_IRQHandler()
{
	// Clear pending interrupt(s)
	TIM2->SR = 0;
}

void __attribute__((interrupt("IRQ"))) EXTI2_IRQHandler()
{
	// Clear pending interrupts
	EXTI->PR 	= 0;

	// Reset TIM2
	TIM2->EGR	= TIM_EventSource_Update;

	toggleLed1();
}

void initVideo()
{
	initRCC();
	initSyncPort();
	initPixelPort();
	initHSyncCount();
	initPixelClock();
	initPixelDma();

	printf("Video configured\r\n");
}
